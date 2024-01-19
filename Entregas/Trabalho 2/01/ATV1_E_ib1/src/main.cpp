#include <Arduino.h>

hw_timer_t *timer = NULL;
const int intervaloMicrossegundos = 1000000;

const int ledPins[] = {15, 2, 0, 4};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
int currentLed = 0;

const int potPin = 34;
volatile int valorPot = 0;
volatile unsigned long previousMillis = 0;

void IRAM_ATTR onTimer() {
  valorPot = analogRead(potPin); // Read the potentiometer value
  Serial.print("LED Frequency: ");
  Serial.println(valorPot / 5);
}

void setup() {
  Serial.begin(115200);

  // Initialize the timer
  timer = timerBegin(0, 80, true);

  // Attach the interrupt function to the timer
  timerAttachInterrupt(timer, &onTimer, true);

  // Set the timer frequency (reload interval)
  timerAlarmWrite(timer, intervaloMicrossegundos, true);

  // Enable the timer overflow interrupt
  timerAlarmEnable(timer);

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Check if enough time has passed to switch to the next LED
  if (millis() - previousMillis >= valorPot) {
    previousMillis = millis();

    // Logic to control LEDs
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;
  }
}
