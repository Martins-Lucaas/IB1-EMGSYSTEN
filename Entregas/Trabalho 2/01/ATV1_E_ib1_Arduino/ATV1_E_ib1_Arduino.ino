const int ledPins[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
unsigned long previousMillis = 0;
int currentLed = 0;

const int potPin = A5;
volatile int valorPot = 0;

void leituraPotenciometro();

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), leituraPotenciometro, CHANGE);

  // Configurar temporizador para interrupção por estouro de clock a cada 0.1 segundos
  noInterrupts();  // Desabilitar interrupções durante a configuração
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1562;  // Valor para gerar interrupção a cada 0.1 segundo (com base em uma frequência de 16MHz e prescaler de 1024)
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Configurar prescaler para 1024
  TIMSK1 |= (1 << OCIE1A);  // Habilitar interrupção por comparação
  interrupts();  // Habilitar interrupções após a configuração

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= valorPot / 5) {
    previousMillis = currentMillis;

    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;
  }

  Serial.print("Potentiometer Reading: ");
  Serial.println(valorPot);
}

void leituraPotenciometro() {
  valorPot = analogRead(potPin);
}

ISR(TIMER1_COMPA_vect) {
  // Timer interrupt handler
  leituraPotenciometro();  // Executar a leitura do potenciômetro
}
