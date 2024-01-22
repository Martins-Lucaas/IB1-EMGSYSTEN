#include <Arduino.h>

hw_timer_t *timer = NULL;
const int intervaloMicrossegundos = 100000;

const int ledPins[] = {2,30,31,32,33};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
int currentLed = 0;

const int potPin = 4;
volatile int valorPot = 0;
volatile unsigned long previousMillis = 0;

void IRAM_ATTR onTimer() {
  valorPot = analogRead(potPin); // Executar a leitura do potenciômetro
}

void setup() {
  Serial.begin(115200);

  // Inicializar o temporizador
  timer = timerBegin(0, 80, true);

  // Anexar a funcao de interrupcao ao temporizador
  timerAttachInterrupt(timer, &onTimer, true);

  // Definir a frequencia do temporizador (intervalo de recarga)
  timerAlarmWrite(timer, intervaloMicrossegundos, true);

  // Ativar a interrupção por estouro do temporizador
  timerAlarmEnable(timer);

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    Serial.print("Frequencia do LED: ");
    Serial.println(valorPot/5);
  }
}

void loop() {
  // Verifica se tempo suficiente passou para mudar para o próximo LED
  if (millis() - previousMillis >= valorPot)/5 {
    previousMillis = millis();

    // Lógica para acionar LEDs
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;

    
  }
}