const int potPin = 4;  // Pino analógico ao qual o potenciômetro está conectado

hw_timer_t *My_timer = NULL;

volatile int valorPot = 0;
volatile bool shouldPrint = false;

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
}

void loop() {
  if (shouldPrint) {
    Serial.print("Valor do potenciômetro: ");
    Serial.println(valorPot);
    shouldPrint = false;  // Reseta a flag
  }
}
