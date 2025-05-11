#include "OLEDDisplay.h"
#include <Wire.h>
#include <EEPROM.h>
// #include <Adafruit_GFX.h>
#include "Fonts/FreeSansBoldOblique18pt7b.h"
#include "Fonts/FreeSansBoldOblique9pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSans9pt7b.h"

OLEDDisplay::OLEDDisplay(int width, int height, int i2c_addr, int sdaPin, int sckPin)
    : width(width), height(height), i2c_addr(i2c_addr), alarmActive(false), display(Adafruit_SH1106G(width, height, &Wire, -1)), sdaPin(sdaPin), sckPin(sckPin), flaga_OLED_ON_OFF(true)
{
}

bool OLEDDisplay::begin()
{
    Wire.begin(sdaPin, sckPin); // Inicjalizacja I2C
    delay(1000);

    if (display.begin(i2c_addr, true))
    {

        int stan_OLED = EEPROM.read(120);
        if (stan_OLED == 0) // jeśli stan_OLED == 0 to przy włączaniu OLED wyłączony
        {
            display.oled_command(SH110X_DISPLAYOFF);
        }
        else
        {
            display.clearDisplay();
            display.setContrast(1);
            // display.setTextSize(1);
            // display.setTextColor(SH110X_WHITE);
            // display.setCursor(0, 0);
            // display.print("Hello!");
            // display.oled_command(0x81); // Set Contrast Control
            // display.oled_command(0x10);

            display.display();
        }
        return true;
    }
    else
    {
        // Serial.println(F("OLED init failed"));
        return false;
    }
}

void OLEDDisplay::oledOnOff()
{
    int stan_OLED = EEPROM.read(120);
    if (stan_OLED == 1) // jeśli stan_OLED == 1 to wyłącz OLED
    {
        display.oled_command(SH110X_DISPLAYOFF);
        // flaga_OLED_ON_OFF = false;
        EEPROM.put(120, 0);
        EEPROM.commit();
        return;
    }
    else
    {

        display.oled_command(SH110X_DISPLAYON);
        display.clearDisplay();
        // display.clearDisplay();
        display.setContrast(1);
        display.display();
        //  flaga_OLED_ON_OFF = true;
        EEPROM.put(120, 1);
        EEPROM.commit();
    }

    // this->flaga_OLED_ON_OFF = false;
    // Serial.println("Kontrast ustawiony");
}

void OLEDDisplay::startAlarm(int alarmValue, unsigned long onDuration, unsigned long offDuration, unsigned long pauseDuration)
{
    this->alarmValue = alarmValue;
    this->onDuration = onDuration;
    this->offDuration = offDuration;
    this->pauseDuration = pauseDuration;
    this->alarmActive = true;
    this->blinkCount = 0;
    this->previousMillis = millis();
    this->isVisible = true;
    this->state = ALARM_ON; // Ustawienie stanu na włączony alarm
}

void OLEDDisplay::stopAlarm()
{
    this->alarmActive = false;
}

bool OLEDDisplay::isAlarmActive()
{
    return alarmActive;
}

void OLEDDisplay::update(const String &currentTime, const String &currentSec, const String &formattedDate)
{

    // display.oled_command(0x81); // Set Contrast Control
    // display.oled_command(0x10);
    display.clearDisplay();

    if (alarmActive)
    {
        displayAlarmValue(); // Miganie wartości alarmu
    }
    else
    {
        displayTime(currentTime, currentSec, formattedDate); // Wyświetlanie aktualnego czasu
    }

    display.display(); // Aktualizacja ekranu
}

void OLEDDisplay::displayTime(const String &currentTime, const String &currentSec, const String &formattedDate)
{
    display.setFont(&FreeSansBold18pt7b);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    // display.setCursor(2, 43);
    display.setCursor(5, 33);
    display.print(currentTime);
    display.setFont(&FreeSansBold9pt7b);
    display.setTextSize(1);
    display.print(currentSec);
    display.setCursor(21, 61);
    display.setFont(&FreeSans9pt7b);
    display.print(formattedDate);
}

// Poprawiona metoda do obsługi alarmu bez blokujących funkcji
void OLEDDisplay::displayAlarmValue()
{
    unsigned long currentMillis = millis();

    switch (state)
    {
    case ALARM_ON:
        if (currentMillis - previousMillis >= onDuration)
        {
            isVisible = false;
            previousMillis = currentMillis;
            state = ALARM_OFF; // Zmiana stanu na wyłączony alarm
        }
        break;

    case ALARM_OFF:
        if (currentMillis - previousMillis >= offDuration)
        {
            isVisible = true;
            previousMillis = currentMillis;
            blinkCount++; // Zwiększenie licznika mignięć

            if (blinkCount >= alarmValue)
            {
                state = ALARM_PAUSE; // Zmiana stanu na pauzę
                blinkCount = 0;      // Resetowanie licznika mignięć
            }
            else
            {
                state = ALARM_ON; // Zmiana stanu na włączony alarm
            }
        }
        break;

    case ALARM_PAUSE:
        if (currentMillis - previousMillis >= pauseDuration)
        {
            previousMillis = currentMillis;
            state = ALARM_ON; // Po pauzie zaczynamy od nowa
        }
        break;
    }

    // Wyświetlanie liczby tylko wtedy, gdy jest widoczna
    if (isVisible)
    {
        display.setFont(&FreeSansBoldOblique18pt7b);
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(45, 35);
        display.print(alarmValue);
    }
}