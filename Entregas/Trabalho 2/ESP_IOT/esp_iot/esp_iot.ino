#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Martins WiFi6";
const char* password = "17031998";

const int ledPins[] = {5,18,19,21};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
int currentLed = 0;

String slider_value = "0";

const char* input_parameter = "value";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Blink Speed Control Web Server</title>
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #38c0ff  ;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background:#01070a; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #01070a; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>Controle da Velocidade do LED pela WEB para ESP32</h2>
  <p>Intervalo de Piscada:  <span id="textslider_value">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderSpeed(this)" id="speedSlider" min="1" max="2000" value="%SLIDERVALUE%" step="1" class="slider"></p>
<script>
function updateSliderSpeed(element) {
  var slider_value = document.getElementById("speedSlider").value;
  document.getElementById("textslider_value").innerHTML = slider_value;
  console.log(slider_value);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+slider_value, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String processor(const String& var){
  if (var == "SLIDERVALUE"){
    return slider_value;
  }
  return String();
}

volatile int valorPot = 0;
volatile bool shouldPrint = false;

void IRAM_ATTR onTimer() {
  valorPot = analogRead(potPin);  // Lê o valor do potenciômetro
  shouldPrint = true;  // Sinaliza que o valor deve ser impresso no loop principal
}

void setup(){
  Serial.begin(115200);

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 100000, true);
  timerAlarmEnable(My_timer);  // Ativa o temporizador

    for (int i = 0; i < numLeds; i++) {
      pinMode(ledPins[i], OUTPUT);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(input_parameter)) {
      message = request->getParam(input_parameter)->value();
      slider_value = message;
    }
    else {
      message = "No message sent";
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });
  
  server.begin();
}

void loop() {
  static unsigned long previousMillis = 0;
  int interval = slider_value.toInt();
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
        // Lógica para acionar LEDs
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    digitalWrite(ledPins[currentLed], HIGH);

    currentLed = (currentLed + 1) % numLeds;
  }
}