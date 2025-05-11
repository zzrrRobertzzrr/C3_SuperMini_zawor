#include "BuzzerPlayer.h"

BuzzerPlayer::BuzzerPlayer(int pin, unsigned long toneTime, unsigned long pauseTime)
{
    buzzerPin = pin;
    toneDuration = toneTime;
    pauseDuration = pauseTime;
    buzerState = LOW;
    currentTone = 0;
    previousMillis = 0;
    buzzingActive = false;
    toneCount = 0; //Liczba dźwięków do wydania

    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, buzerState); // Ustaw buzzer jako wyłączony na początku
}

void BuzzerPlayer::start()
{
    buzzingActive = true;
    currentTone = 0;
    previousMillis = millis();
}

void BuzzerPlayer::stop()
{
    buzzingActive = false;
    digitalWrite(buzzerPin, LOW); // Wyłącz buzzer
}

void BuzzerPlayer::update()
{
    currentMillis = millis();

    if (buzzingActive && toneCount > 0)
    {
        if (currentTone < toneCount)
        {
            if (currentMillis - previousMillis >= toneDuration)
            {
                buzerState = !buzerState;
                digitalWrite(buzzerPin, buzerState); // Włącz buzzer
                previousMillis = currentMillis;
                if (buzerState == LOW)
                {
                    currentTone++;
                }
            }
        }
        else
        {
            if (currentMillis - previousMillis >= pauseDuration)
            {
                // digitalWrite(buzzerPin, LOW); // Wyłącz buzzer
                currentTone = 0;
                previousMillis = currentMillis;
            }
        }
    }
}