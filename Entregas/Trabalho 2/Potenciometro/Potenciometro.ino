const int potPin = 4;  // Pino analógico ao qual o potenciômetro está conectado

hw_timer_t *My_timer = NULL;

volatile int valorPot = 0;
volatile bool shouldPrint = false;

const int ledPins[] = {5,18,19,21};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
int currentLed = 0;

void IRAM_ATTR onTimer() {
  valorPot = analogRead(potPin);  // Lê o valor do potenciômetro
  shouldPrint = true;  // Sinaliza que o valor deve ser impresso no loop principal

}

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 100000, true);
  timerAlarmEnable(My_timer);  // Ativa o temporizador

   for (int i = 0; i < numLeds; i++) {
      pinMode(ledPins[i], OUTPUT);
  }



}

void loop() {
  if (shouldPrint) {
    Serial.print("Valor do potenciômetro: ");
    Serial.println(valorPot);
    Serial.print("Frequencia de alternancia do LED em ms: ");
    Serial.println(valorPot/5);
    shouldPrint = false;  // Reseta a flag
  }

  static unsigned long previousMillis = 0;
  
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= valorPot/5) {
    previousMillis = currentMillis;
        // Lógica para acionar LEDs
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;
  }

}
