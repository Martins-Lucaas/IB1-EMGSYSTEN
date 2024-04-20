#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

const char *ssid = "Martins WiFi6";
const char *password = "17031998";

WebServer server(80);

const int bufferSize = 250;
float timeElapsed = 0;
int emgBuffer[bufferSize];
int bufferIndex = 0;
bool updatingData = false;
bool tableEnabled = false;
unsigned long lastUpdateTime = 0;
unsigned long lastReadTime = 0; // Variável para controlar o intervalo de leitura da resistência
const unsigned long readInterval = 500; // Intervalo de leitura da resistência em milissegundos

const int pinEMG = 33; // Porta do ESP conectada AO EMG
const int pinPot = 35; // Conectada ao Potenciometro 

int readEMGValue() {
  return analogRead(pinEMG);
}

void handleRoot() {
 String html =
      "<!DOCTYPE HTML>"
      "<html>"
      "<head>"
      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<title>Projeto IB1</title>"
      "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
      "<style>"
      "body {"
      "font-family: Arial, sans-serif;"
      "margin: 0;"
      "padding: 0;"
      "background-color: #222;"
      "color: #eee;"
      "}"
      ".container {"
      "margin: 50px auto;"
      "padding: 20px;"
      "background-color: #333;"
      "border-radius: 10px;"
      "box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
      "text-align: center;"
      "}"
      ".container h2 {"
      "font-size: 32px;"
      "color: #eee;"
      "}"
      ".container p {"
      "font-size: 18px;"
      "color: #ddd;"
      "}"
      ".container canvas {"
      "width: 3000;" // Largura aumentada para três vezes a largura do contêiner
      "height: 1000;"
      "margin: 20px auto;"
      "}"
      ".button {"
      "border-radius: 20px;"
      "padding: 10px 20px;"
      "margin: 10px;"
      "cursor: pointer;"
      "border: none;"
      "color: #eee;"
      "font-size: 16px;"
      "transition: background-color 0.1s ease, transform 0.1s ease;"
      "}"
      ".button.start {"
      "background-color: #4CAF50;"
      "}"
      ".button.start:hover {"
      "background-color: #45a049;"
      "}"
      ".button.start:active {"
      "transform: scale(0.95);"
      "}"
      ".button.stop {"
      "background-color: #f44336;"
      "}"
      ".button.stop:hover {"
      "background-color: #d32f2f;"
      "}"
      ".button.stop:active {"
      "transform: scale(0.95);"
      "}"
      ".button.save {"
      "background-color: #008CBA;"
      "}"
      ".button.save:hover {"
      "background-color: #0077a3;"
      "}"
      ".button.save:active {"
      "transform: scale(0.95);"
      "}"
      ".button.table {"
      "background-color: #ff9800;"
      "}"
      ".button.table:hover {"
      "background-color: #f57c00;"
      "}"
      ".button.table:active {"
      "transform: scale(0.95);"
      "}"
      ".hidden {"
      "display: none;"
      "}"
      "</style>"
      "</head>"
      "<body>"
      "<div class='container'>"
      "<h2>Projeto IB1</h2>"
      "<p>Valor do EMG: <span id='emg_value'>0</span></p>"
      "<canvas id='potChart' width='3000' height='auto'></canvas>"
      "<button id='startButton' class='button start' onclick='start()' disabled>Iniciar</button>"
      "<button class='button stop' onclick='stop()'>Parar</button>"
      "<button class='button save' onclick='save()'>Salvar</button>"
      "<button id='showTableButton' class='button table' onclick='showTable()' disabled>Ver Tabela</button>"
      "<div id='tableContainer' class='hidden'>"
      "<table>"
      "<thead>"
      "<tr>"
      "<th>Index</th>"
      "<th>Value</th>"
      "</tr>"
      "</thead>"
      "<tbody id='tableBody'></tbody>"
      "</table>"
      "</div>"
      "</div>"
      "<script>"
      "var ctx = document.getElementById('potChart').getContext('2d');"
      "var potChart = new Chart(ctx, {"
      "  type: 'line',"
      "  data: {"
      "    labels: [],"
      "    datasets: [{"
      "      label: 'Amplitude EMG',"
      "      data: [],"
      "      fill: false,"
      "      borderColor: '#7FFFD4',"
      "      backgroundColor: 'rgba(75, 192, 192, 0.2)',"
      "      tension: 0.1"
      "    }]"
      "  },"
      "  options: {"
      "    animation: {"
      "      duration: 0" // Desativa a animação
      "    },"
      "    scales: {"
      "      x: {"
      "        display: false,"
      "      },"
      "      y: {"
      "        suggestedMin: 0,"
      "        suggestedMax: 4095"
      "      }"
      "    }"
      "  }"
      "});"
      "var timeElapsed = 0;"
      "var intervalId;"
      "var dataArray = [];"
      "function updatePotChart(emgvalue) {"
      "  dataArray.push({x: (timeElapsed * 250).toFixed(1), y: emgvalue});"
      "  timeElapsed += 0.1;"
      
      "  if (dataArray.length > " + String(bufferSize) + ") {"
      "    dataArray.shift();" // Remove o primeiro elemento do vetor
      "    potChart.data.labels.shift();" // Remove o primeiro rótulo
      "    potChart.data.datasets[0].data.shift();" // Remove o primeiro valor do conjunto de dados
      "  }"
      
      "  potChart.data.labels.push(dataArray.length);"
      "  potChart.data.datasets[0].data.push(emgvalue);"
      "  potChart.update();"
      "}"
      "function start() {"
      "  if (!updatingData) {"
      "    intervalId = setInterval(function() {"
      "      fetch('/emgvalue')"
      "      .then(response => response.text())"
      "      .then(data => {"
      "        document.getElementById('emg_value').innerText = data;"
      "        updatePotChart(parseInt(data));"
      "      });"
      "    }, 1);"
      "    document.getElementById('startButton').disabled = true;"
      "    document.getElementById('showTableButton').disabled = false;"
      "    updatingData = true;"
      "  }"
      "}"
      "function stop() {"
      "  clearInterval(intervalId);"
      "  document.getElementById('startButton').disabled = false;"
      "  updatingData = false;"
      "}"
      "function save() {"
      "  alert('Dados salvos!');"
      "}"
      "function showTable() {"
      "  var tableContainer = document.getElementById('tableContainer');"
      "  if (tableContainer.classList.contains('hidden')) {"
      "    tableContainer.classList.remove('hidden');"
      "    document.getElementById('showTableButton').innerText = 'Ocultar Tabela';"
      "    fillTable();"
      "  } else {"
      "    tableContainer.classList.add('hidden');"
      "    document.getElementById('showTableButton').innerText = 'Ver Tabela';"
      "  }"
      "}"
      "function fillTable() {"
      "  var tableBody = document.getElementById('tableBody');"
      "  tableBody.innerHTML = '';"
      "  dataArray.forEach(function(data, index) {"
      "    var row = tableBody.insertRow();"
      "    var indexCell = row.insertCell(0);"
      "    var valueCell = row.insertCell(1);"
      "    indexCell.innerText = index + 1;"
      "    valueCell.innerText = data.y;"
      "  });"
      "}"
      "</script>"
      "</body>"
      "</html>";
  server.send(200, "text/html", html);
}

void setup() {
  pinMode(pinEMG, INPUT);
  pinMode(pinPot, INPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/emgvalue", HTTP_GET, []() {
    server.send(200, "text/plain", String(readEMGValue()));
  });

  server.begin();

  pinMode(32, INPUT);

  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  
  if (millis() - lastUpdateTime >= 1) {
    lastUpdateTime = millis();
    if (updatingData) {
      int emgvalue = readEMGValue();
      server.send(200, "text/plain", String(emgvalue));
    }
  }

  // Intervalo para leitura da resistência a cada 0.5 segundo
  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();
    int valorADC = analogRead(pinPot); // Lê o valor analógico na porta do potenciômetro
    float resistencia = map(valorADC, 0, 4095, 0, 100000); // Mapeia o valor ADC para um intervalo de resistência
    Serial.println(resistencia); // Imprime a resistência do potenciômetro
  }
}
