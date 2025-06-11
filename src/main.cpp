#include <Arduino.h>
#include <esp_wifi.h> // Do zaawansowanego zarządzania WiFi
#include <esp_bt.h>
#include <esp_sleep.h>
#include <HardwareSerial.h>

#define HC12_RX 20      // GPIO20 do HC-12 TX
#define HC12_TX 21      // GPIO21 do HC-12 RX
HardwareSerial HC12(1); // UART1

#define pinQ4 7           // Q4 odcięcie zasilania od sekcji zasilania przekaźnika i silnika
#define pinRelay 10       // wł/wył przekaźnik
#define pinLimitSwitch1 1 // krańcówka
#define pinLimitSwitch2 2 // krańcówka

#define LED 8                    // Define the LED_BUILTIN pin number
bool Flaga_LimitSwitch1 = false; // Flaga krańcówki 1
bool Flaga_LimitSwitch2 = false; // Flaga krańcówki 2

void setup()
{
  Serial.begin(9600);
  HC12.begin(9600, SERIAL_8N1, HC12_RX, HC12_TX); // HC-12 przez UART1
  pinMode(LED, OUTPUT);
  esp_wifi_deinit(); // Zwolnienie zasobów
  btStop();          // Funkcja z esp_bt.h
  esp_bt_controller_disable();

  pinMode(pinQ4, OUTPUT);                 // Ustawienie pinu Q4 jako wyjście
  pinMode(pinRelay, OUTPUT);              // Ustawienie pinu przekaźnika jako wyjście
  pinMode(pinLimitSwitch1, INPUT_PULLUP); // Ustawienie pinu krańcówki jako wejście z podciąganiem
  pinMode(pinLimitSwitch2, INPUT_PULLUP); // Ustawienie pinu krańcówki jako wejście z podciąganiem
  digitalWrite(pinQ4, LOW);               // Ustawienie pinu Q4 w stan niski (odcięcie GND od sekcji zasilania przekaźnika i silnika)
  digitalWrite(pinRelay, HIGH);           // Ustawienie pinu przekaźnika w stan niski (wyłączenie przekaźnika)

  // esp_sleep_enable_uart_wakeup(0); // 0 = UART0, czyli Serial na RX0

  // Serial.println("Start systemu – debug USB"); // UART0
  // HC12.println("Hello HC-12");                 // UART1
}

// the loop function runs over and over again forever
void loop()
{
  if (HC12.available())
  {
    String data = HC12.readStringUntil('\n');
    data.trim();
    if (data.startsWith("CZW:"))
    {
    }
    if (data == "ZAW:close")
    {
      digitalWrite(pinQ4, HIGH);
      digitalWrite(pinRelay, HIGH); // Wyłącz przekaźnik
      Serial.println("zaw_close");
      Flaga_LimitSwitch1 = true;
    }
    if (data == "ZAW:open")
    {
      digitalWrite(pinQ4, HIGH);
      digitalWrite(pinRelay, LOW); // Włącz przekaźnik
      Serial.println("zaw_open");
      Flaga_LimitSwitch2 = true;
    }
    data = "";
    // Serial.print("Z HC-12: ");
    // Serial.println(data);
  }

  if (/*digitalRead(pinRelay) == HIGH && */ Flaga_LimitSwitch1 == true && digitalRead(pinLimitSwitch1) == LOW /* && digitalRead(pinLimitSwitch2) == LOW*/)
  {
    digitalWrite(pinQ4, LOW); // Wyłącz masę
    Serial.println("Zawór zamkniety");
    HC12.println("ZAW:closed");
    Flaga_LimitSwitch1 = false; // Reset flagi krańcówki 1
  }
  else if (/*digitalRead(pinRelay) == LOW && */ Flaga_LimitSwitch2 == true && digitalRead(pinLimitSwitch2) == LOW /* && digitalRead(pinLimitSwitch2) == HIGH*/)
  {
    digitalWrite(pinQ4, LOW); // Wyłącz masę
    Serial.println("Zawór otwarty");
    HC12.println("ZAW:opened");
    Flaga_LimitSwitch2 = false; // Reset flagi krańcówki 2
  }

  // static unsigned long lastSend2 = 0;
  // if (millis() - lastSend2 > 20000)
  // {
  //   HC12.println("Zamykam zawór");
  //   Serial.println("Zamykam zawór");
  //   lastSend2 = millis();
  //   digitalWrite(pinRelay, !digitalRead(pinRelay)); // Włącz przekaźnik
  // }
}