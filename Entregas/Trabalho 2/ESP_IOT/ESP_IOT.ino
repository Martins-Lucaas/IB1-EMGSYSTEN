#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Martins WiFi6";
const char* password = "17031998";

const int ledPins[] = {5, 18, 19, 21};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
int currentLed = 0;

String slider_value = "0";
int analogValue = 0;

const char* input_parameter = "value";

AsyncWebServer server(80);

#include "index.html"

String processor(const String& var){
  if (var == "SLIDERVALUE"){
    return slider_value;
  }
  return String();
}

void setup(){
  Serial.begin(115200);

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

  server.on("/analog", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(analogRead(4))); // Pino D2
  });

  server.begin();
}

void loop() {
  static unsigned long previousMillis = 0;
  int interval = slider_value.toInt();

  // Garante que o intervalo está dentro dos limites
  interval = constrain(interval, 1, 1000);

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
