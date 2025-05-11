#ifndef BUZZERPLAYER_H
#define BUZZERPLAYER_H

#include <Arduino.h>

class BuzzerPlayer
{
private:
    int buzzerPin;               // Pin, do którego podłączony jest buzzer
    unsigned long toneDuration;  // Czas trwania jednego dźwięku
    unsigned long pauseDuration; // Czas przerwy między dźwiękami
    int buzerState;
    int currentTone;
    // Flaga określająca, czy buzzer jest aktywny
    unsigned long previousMillis;
    unsigned long currentMillis;
    bool buzzingActive;

public:
    int toneCount; // Liczba dźwięków do wydania

    BuzzerPlayer(int pin, unsigned long toneTime, unsigned long pauseTime);

    void start();  // Rozpocznij wydawanie dźwięków
    void stop();   // Zatrzymaj wydawanie dźwięków
    void update(); // Funkcja zarządzająca wydawaniem dźwięków
};

#endif