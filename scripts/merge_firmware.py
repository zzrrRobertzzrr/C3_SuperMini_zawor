Import("env")
import csv
import os
import subprocess
import sys

platform = env.PioPlatform()
esptool = os.path.join(platform.get_package_dir("tool-esptoolpy"), "esptool.py")

build_dir = env.subst("$BUILD_DIR")
progname = env.subst("${PROGNAME}")
board_cfg = env.BoardConfig()
mcu = (board_cfg.get("build.mcu") or "").lower()

if "esp32c3" in mcu:
    chip = "esp32c3"
    BOOT_OFFSET = "0x0"
elif "esp32s2" in mcu:
    chip = "esp32s2"
    BOOT_OFFSET = "0x0"
elif "esp32s3" in mcu:
    chip = "esp32s3"
    BOOT_OFFSET = "0x0"
else:
    chip = "esp32"
    BOOT_OFFSET = "0x1000"

PART_OFFSET = "0x8000"
DEFAULT_APP_OFFSET = "0x10000"
DEFAULT_OTA_OFFSET = "0xE000" if chip in ("esp32c3", "esp32s2", "esp32s3") else "0xD000"


def _to_hex(value):
    try:
        return hex(int(str(value), 0))
    except Exception:
        return None


def _to_int(value):
    try:
        return int(str(value), 0)
    except Exception:
        return None


APP_OFFSET = DEFAULT_APP_OFFSET
OTA_OFFSET = DEFAULT_OTA_OFFSET
FS_OFFSET = None
FS_KIND = None
FS_SIZE = None

csv_opt = env.GetProjectOption("board_build.partitions")
if csv_opt:
    csv_path = csv_opt if os.path.isabs(csv_opt) else os.path.join(env.subst("$PROJECT_DIR"), csv_opt)
    if os.path.exists(csv_path):
        try:
            with open(csv_path, newline="") as handle:
                reader = csv.reader(handle)
                for row in reader:
                    if not row:
                        continue
                    name = row[0].strip()
                    if not name or name.startswith("#") or name.lower() == "name":
                        continue

                    cols = [col.strip() for col in row]
                    while len(cols) < 5:
                        cols.append("")

                    _name, ptype, subtype, offset, size = cols[:5]
                    ptype = ptype.lower()
                    subtype = subtype.lower()

                    if ptype == "app" and subtype in ("factory", "ota_0"):
                        parsed = _to_hex(offset)
                        if parsed is not None:
                            APP_OFFSET = parsed

                    elif ptype == "data" and subtype == "ota":
                        parsed = _to_hex(offset)
                        if parsed is not None:
                            OTA_OFFSET = parsed

                    elif ptype == "data" and subtype in ("spiffs", "littlefs", "fat"):
                        parsed_offset = _to_hex(offset)
                        if parsed_offset is not None:
                            FS_OFFSET = parsed_offset
                            FS_KIND = subtype
                        parsed_size = _to_int(size)
                        if parsed_size is not None:
                            FS_SIZE = parsed_size
        except Exception as exc:
            print(f"Warning: failed to parse partition CSV '{csv_path}': {exc}")


def _create_blank_fs():
    if FS_SIZE is None or FS_SIZE <= 0:
        return None
    filename = f"{FS_KIND or 'fs'}_blank.bin"
    out_path = os.path.join(build_dir, filename)
    try:
        with open(out_path, "wb") as handle:
            handle.write(b"\xFF" * FS_SIZE)
        print(f"Created blank FS image {out_path} ({FS_SIZE} bytes).")
        return out_path
    except Exception as exc:
        print("Warning: failed to create blank FS image:", exc)
        return None


def ensure_fs_image():
    if FS_OFFSET is None:
        return None

    candidates = [
        os.path.join(build_dir, "littlefs.bin"),
        os.path.join(build_dir, "spiffs.bin"),
    ]
    for path in candidates:
        if os.path.exists(path):
            return path

    pioenv = env.subst("$PIOENV")
    projdir = env.subst("$PROJECT_DIR")
    commands = []
    python_exe = env.subst("$PYTHONEXE") or sys.executable
    if python_exe:
        commands.append([python_exe, "-m", "platformio", "run", "-e", pioenv, "-t", "buildfs"])
    commands.append(["platformio", "run", "-e", pioenv, "-t", "buildfs"])
    commands.append(["pio", "run", "-e", pioenv, "-t", "buildfs"])

    for cmd in commands:
        try:
            print("FS image missing, running:", " ".join(cmd))
            subprocess.check_call(cmd, cwd=projdir)
            break
        except Exception as exc:
            print("buildfs attempt failed:", exc)
    else:
        return _create_blank_fs()

    for path in candidates:
        if os.path.exists(path):
            return path

    return _create_blank_fs()


def merge_bins(source, target, env):
    bootloader = os.path.join(build_dir, "bootloader.bin")
    partitions = os.path.join(build_dir, "partitions.bin")
    app = os.path.join(build_dir, f"{progname}.bin")
    out = os.path.join(build_dir, f"{progname}_merged_factory.bin")

    if not (os.path.exists(bootloader) and os.path.exists(partitions) and os.path.exists(app)):
        print("Skip merge: missing one of required images.")
        return

    cmd = [
        env.subst("$PYTHONEXE") or sys.executable,
        esptool,
        "--chip",
        chip,
        "merge_bin",
        "-o",
        out,
        BOOT_OFFSET,
        bootloader,
        PART_OFFSET,
        partitions,
    ]

    ota_init = os.path.join(build_dir, "ota_data_initial.bin")
    boot_app0 = os.path.join(build_dir, "boot_app0.bin")
    if os.path.exists(ota_init):
        cmd += [OTA_OFFSET, ota_init]
    elif os.path.exists(boot_app0):
        cmd += [OTA_OFFSET, boot_app0]

    cmd += [APP_OFFSET, app]

    fs_image = ensure_fs_image()
    if FS_OFFSET and fs_image:
        cmd += [FS_OFFSET, fs_image]
    elif FS_OFFSET and not fs_image:
        print(f"FS image not found; skip {FS_KIND} at {FS_OFFSET}.")

    print("Merging to:", out)
    print("Command:", " ".join(cmd))
    subprocess.check_call(cmd)
    print("Merged image created:", out)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", merge_bins)

