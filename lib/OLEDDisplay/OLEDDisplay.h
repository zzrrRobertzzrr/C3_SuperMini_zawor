#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class OLEDDisplay
{
public:
    OLEDDisplay(int width, int height, int i2c_addr, int sdaPin, int sckPin);
    bool begin();                                                                                                      // Inicjalizacja wyświetlacza
    void startAlarm(int alarmValue, unsigned long onDuration, unsigned long offDuration, unsigned long pauseDuration); // Uruchomienie alarmu
    void stopAlarm();                                                                                                  // Zatrzymanie alarmu
    bool isAlarmActive();                                                                                              // Sprawdzanie, czy alarm jest aktywny
    void update(const String &currentTime, const String &currentSec, const String &formattedDate);
    void oledOnOff();
    // Aktualizacja ekranu (godzina lub alarm)

private:
    Adafruit_SH1106G display;
    int width, height;
    int i2c_addr;
    int sdaPin, sckPin;
    bool flaga_OLED_ON_OFF;

    bool alarmActive;            // Czy alarm jest aktywny
    int alarmValue;              // Wartość alarmu, która będzie migać
    unsigned long onDuration;    // Czas wyświetlania liczby
    unsigned long offDuration;   // Czas przerwy między mignięciami
    unsigned long pauseDuration; // Czas przerwy po serii mignięć

    unsigned long previousMillis; // Do zarządzania czasem
    unsigned long blinkCount;     // Ilość mignięć alarmu
    bool isVisible;               // Czy liczba jest aktualnie wyświetlana

    enum AlarmState
    {
        ALARM_ON,
        ALARM_OFF,
        ALARM_PAUSE
    };
    AlarmState state; // Stan alarmu (miganie lub pauza)

    void displayTime(const String &currentTime, const String &currentSec, const String &formattedDate); // Wyświetlanie czasu
    void displayAlarmValue();                                                                           // Miganie wartości alarmu
};

#endif // OLED_DISPLAY_H