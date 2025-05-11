#include "LedBlinker.h" // Dołączamy nasz plik nagłówkowy

// Konstruktor klasy
LedBlinker::LedBlinker(int pin, unsigned long blinkTime, unsigned long pauseTime)
{
    ledPin = pin;
    blinkDuration = blinkTime;
    pauseDuration = pauseTime;
    ledState = LOW;
    currentBlink = 0;
    previousMillis = 0;
    blinkingActive = false; // Na początku miganie nie jest aktywne
    blinkCount = 0;         // Początkowa liczba mignięć

    // Ustawiamy pin jako wyjście
    pinMode(ledPin, OUTPUT);
    // Ustawiamy diodę w stanie wyłączonym
    digitalWrite(ledPin, ledState);
}

// Metoda do uruchomienia migania
void LedBlinker::start()
{
    blinkingActive = true;
    currentBlink = 0;          // Resetowanie licznika mignięć
    previousMillis = millis(); // Ustawienie czasu początkowego
}

// Metoda do zatrzymania migania
void LedBlinker::stop()
{
    blinkingActive = false;
    digitalWrite(ledPin, LOW); // Wyłącz diodę, jeśli miganie zostanie zatrzymane
}

// Funkcja do zarządzania mignięciami diody
void LedBlinker::update()
{
    // Pobieramy bieżący czas
    currentMillis = millis();

    // Jeśli miganie jest aktywne i liczba mignięć większa od zera
    if (blinkingActive && blinkCount > 0)
    {
        // Jeśli jesteśmy w trakcie serii mignięć
        if (currentBlink < blinkCount)
        {
            // Sprawdzamy, czy minął czas dla jednego mignięcia
            if (currentMillis - previousMillis >= blinkDuration)
            {
                // Zmieniamy stan diody
                ledState = !ledState;
                digitalWrite(ledPin, ledState);

                // Aktualizujemy czas ostatniej zmiany stanu diody
                previousMillis = currentMillis;

                // Zwiększamy licznik mignięć, jeśli dioda została wyłączona
                if (ledState == LOW)
                {
                    currentBlink++;
                }
            }
        }
        else
        {
            // Jeśli seria mignięć jest zakończona, czekamy na pauzę
            if (currentMillis - previousMillis >= pauseDuration)
            {
                // Resetujemy licznik mignięć i zaczynamy od nowa
                currentBlink = 0;
                previousMillis = currentMillis;
            }
        }
    }
}