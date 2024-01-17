const int ledPins[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
unsigned long previousMillis = 0;
int currentLed = 0;

const int potPin = A5;

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), leituraPotenciometro, CHANGE);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  int valorPot = analogRead(potPin);
  Serial.println("Valor do potenciometro: " + String(valorPot));

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= valorPot/5) {
    previousMillis = currentMillis;

    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;
  }
}

void leituraPotenciometro() {
  //GENTE MUDA AQUI
}
