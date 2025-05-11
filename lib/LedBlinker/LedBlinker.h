/*
Tworzymy obiekt klasy LedBlinker
Parametry: (pin diody, czas jednego mignięcia w ms, czas przerwy między seriami w ms)
pod klasą
LedBlinker led(1, 200, 1000);
Na początku Loop() dodajemy led.update();
W Loop() dodajemy led.blinkCount = /jakaś wartość lub zmienna/;
led.start();
led.stop();
 */

#ifndef LEDBLINKER_H
#define LEDBLINKER_H

#include <Arduino.h> // Musimy dołączyć bibliotekę Arduino, aby móc korzystać z funkcji takich jak pinMode, millis itp.

class LedBlinker
{
private:
    int ledPin;                   // Pin, do którego podłączona jest dioda LED
    unsigned long blinkDuration;  // Czas trwania jednego mignięcia
    unsigned long pauseDuration;  // Czas przerwy między seriami mignięć
    int ledState;                 // Stan diody (ON/OFF)
    int currentBlink;             // Aktualna liczba mignięć
    unsigned long previousMillis; // Czas ostatniego mignięcia
    unsigned long currentMillis;  // Aktualny czas
    bool blinkingActive;          // Flaga określająca, czy miganie jest aktywne

public:
    int blinkCount; // Liczba mignięć w serii, publiczna zmienna do dynamicznej zmiany

    // Konstruktor klasy
    LedBlinker(int pin, unsigned long blinkTime, unsigned long pauseTime);

    // Metoda do uruchomienia migania
    void start();

    // Metoda do zatrzymania migania
    void stop();

    // Funkcja do zarządzania mignięciami diody
    void update();
};
#endif