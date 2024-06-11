#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

// Definição das credenciais de WiFi
const char *ssid = "Martins WiFi6";
const char *password = "17031998";

// Inicialização do servidor web na porta 80
WebServer server(80);

// Definição do tamanho do buffer para leitura do sinal EMG
const int bufferSize = 250;

// Variáveis relacionadas à leitura e atualização dos dados do EMG
float timeElapsed = 0;
int emgBuffer[bufferSize];
int bufferIndex = 0;
bool updatingData = false;
bool tableEnabled = false;
bool fftEnabled = false; // Flag para indicar se a plotagem do FFT está habilitada
unsigned long lastUpdateTime = 0;
const int pinEMG = 33;
const int pinPot = 35;

// Função para ler o valor do sinal EMG
float readEMGValue() {
  int valorADC = analogRead(pinEMG);
  float tensao = ((valorADC * 3.3) / 4095) / 660; // Convertendo para volts
  return tensao * 1000; // Convertendo para milivolts
}

// Função para lidar com requisições na rota principal ("/")
void handleRoot() {
 String html =
      "<!DOCTYPE HTML>"
      "<html>"
      "<head>"
      "<title>Projeto IB1</title>"
      "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
      "<script src='https://cdnjs.cloudflare.com/ajax/libs/fft-js/1.0.0/fft.min.js'></script>"
      "<style>"
      "body {"
      "font-family: Arial, sans-serif;"
      "margin: 0;"
      "padding: 0;"
      "background-color: #f0f0f0;"
      "color: #333;"
      "}"
      ".container {"
      "margin: 50px auto;"
      "padding: 20px;"
      "background-color: #fff;"
      "border-radius: 10px;"
      "box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
      "text-align: center;"
      "max-width: 90%;"
      "}"
      ".container h2 {"
      "font-size: 32px;"
      "color: #333;"
      "}"
      ".container p {"
      "font-size: 18px;"
      "color: #666;"
      "}"
      ".container canvas {"
      "width: 100%;"
      "height: 300px;"
      "margin: 20px auto;"
      "border: 1px solid #000;"
      "}"
      "#resistanceBox {"
      "position: absolute;"
      "top: 10px;"
      "left: 10px;"
      "padding: 5px;"
      "border: 1px solid #000;"
      "border-radius: 5px;"
      "background-color: #fff;"
      "}"
      ".button {"
      "border-radius: 20px;"
      "padding: 10px 20px;"
      "margin: 10px;"
      "cursor: pointer;"
      "border: none;"
      "color: #fff;"
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
      ".button.fft {"
      "background-color: #c49200;" // Cor dourada um pouco mais escura para o botão de plotagem do FFT
      "}"
      ".button.fft:hover {"
      "background-color: #ab7d00;"
      "}"
      ".button.fft:active {"
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
      "<p>Valor do EMG: <span id='emg_value'>0</span> mV</p>" // Exibe o valor do EMG em milivolts
      "<canvas id='potChart' class='potChart' width='2000' height='300'></canvas>"
      "<canvas id='fftChart' class='potChart hidden' width='2000' height='300'></canvas>"
      "<div id='resistanceBox'>Ganho Total no circuito</div>" // Exibe o valor do ganho
      "<button id='startButton' class='button start' onclick='start()' disabled>Iniciar</button>"
      "<button class='button stop' onclick='stop()'>Parar</button>"
      "<button class='button save' onclick='save()'>Salvar</button>"
      "<button id='showTableButton' class='button table' onclick='showTable()' disabled>Ver Tabela</button>"
      "<button id='fftButton' class='button fft' onclick='startFFT()'>Espectro de Frequencia</button>" // Botão para plotar o espectro de frequência
      "<div id='tableContainer' class='hidden'>"
      "<table>"
      "<thead>"
      "<tr>"
      "<th>Indice</th>"
      "<th>Valor</th>"
      "</tr>"
      "</thead>"
      "<tbody id='tableBody'></tbody>"
      "</table>"
      "</div>"
      "</div>"
      "<script>"
      "var ctx = document.getElementById('potChart').getContext('2d');"
      "var potChart;"
      "var fftChart;"
      "var timeElapsed = 0;"
      "var intervalId;"
      "var dataArray = [];"
      "function calculateMagnitude(spectrum) {"
      "  return spectrum.map(value => Math.sqrt(value.real * value.real + value.imag * value.imag));"
      "}"
      "function startFFTChart() {"
      "  var fftCtx = document.getElementById('fftChart').getContext('2d');"
      "  fftChart = new Chart(fftCtx, {"
      "    type: 'line',"
      "    data: {"
      "      labels: [],"
      "      datasets: [{"
      "        label: 'FFT Magnitude',"
      "        data: [],"
      "        fill: false,"
      "        borderColor: '#c49200',"
      "        tension: 0.1"
      "      }]"
      "    },"
      "    options: {"
      "      animation: {"
      "        duration: 0"
      "      },"
      "      scales: {"
      "        x: {"
      "          display: false,"
      "        },"
      "        y: {"
      "          suggestedMin: 0,"
      "          suggestedMax: 20"
      "        }"
      "      }"
      "    }"
      "  });"
      "}"
      "function startFFT() {"
      "  if (!updatingData) {"
      "    fftEnabled = true;"
      "    document.getElementById('fftButton').disabled = true;"
      "    startFFTChart();"
      "  }"
      "}"
      "function updateFFTChart(spectrum) {"
      "  const magnitudes = calculateMagnitude(spectrum);"
      "  fftChart.data.labels = magnitudes.map((_, index) => index + 1);"
      "  fftChart.data.datasets[0].data = magnitudes;"
      "  fftChart.update();"
      "}"
      "function updatePotChart(emgvalue) {"
      "  var emgMicrovolts = emgvalue.toFixed(6);"
      "  dataArray.push({x: (timeElapsed * 1000).toFixed(1), y: emgMicrovolts});"
      "  timeElapsed += 0.5;" // Alteração para tempo em milissegundos
      "  if (dataArray.length > " + String(bufferSize) + ") {"
      "    dataArray.shift();"
      "    potChart.data.labels.shift();"
      "    potChart.data.datasets[0].data.shift();"
      "  }"
      "  potChart.data.labels.push(dataArray[dataArray.length - 1].x);"
      "  potChart.data.datasets[0].data.push(emgMicrovolts);"
      "  potChart.update();"
      "  if (fftEnabled) {" // Verifica se a plotagem do FFT está habilitada
      "    const spectrum = calculateFFT(dataArray);"
      "    updateFFTChart(spectrum);"
      "  }"
      "}"
      "function start() {"
      "  if (!updatingData) {"
      "    var potCtx = document.getElementById('potChart').getContext('2d');"
      "    potChart = new Chart(potCtx, {"
      "      type: 'line',"
      "      data: {"
      "        labels: [],"
      "        datasets: [{"
      "          label: 'Amplitude EMG',"
      "          data: [],"
      "          fill: false,"
      "          borderColor: '#4CAF50',"
      "          backgroundColor: 'rgba(75, 192, 192, 0.2)',"
      "          tension: 0.1"
      "        }]"
      "      },"
      "      options: {"
      "        animation: {"
      "          duration: 0"
      "        },"
      "        scales: {"
      "          x: {"
      "            display: false,"
      "          },"
      "          y: {"
      "            suggestedMin: 0,"
      "            suggestedMax: 5" // Máximo de 5mV
      "          }"
      "        }"
      "      }"
      "    });"
      "    document.getElementById('resistanceBox').innerText = 'Valor do Ganho: 0 x';" // Atualizando o valor da resistência para 0 Ohms
      "    intervalId = setInterval(function() {"
      "      fetch('/emgvalue')"
      "      .then(response => response.text())"
      "      .then(data => {"
      "        document.getElementById('emg_value').innerText = data;"
      "        updatePotChart(parseFloat(data));"
      "      });"
      "      fetch('/resistancevalue')"
      "      .then(response => response.text())"
      "      .then(data => {"
      "        document.getElementById('resistanceBox').innerText = 'Valor do ganho: ' + data + ' x';" // Atualizando o valor da resistência
      "      });"
      "    }, 0.5);"
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

// Função de configuração inicial do dispositivo
void setup() {
  pinMode(pinEMG, INPUT);
  pinMode(pinPot, INPUT);
  Serial.begin(115200);

  // Conecta-se à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado ao WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Define as rotas para o servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/emgvalue", HTTP_GET, []() {
    server.send(200, "text/plain", String(readEMGValue(), 6)); // Precisão de 6 casas decimais
  });
  server.on("/resistancevalue", HTTP_GET, []() {
    int valorADC = analogRead(pinPot);
    Serial.println(valorADC);
    int resistencia = map(valorADC, 0, 4095, 0, 100000);
    float ganho = (resistencia/470)*6.6;
    server.send(200, "text/plain", String(ganho)); 
  });

  // Inicia o servidor web
  server.begin();

  Serial.println("Servidor iniciado");
}

// Função principal que é executada repetidamente
void loop() {
  // Manipula as requisições do cliente
  server.handleClient();
  
  // Verifica se é hora de atualizar os dados do EMG
  if (millis() - lastUpdateTime >= 0.5) {
    lastUpdateTime = millis();
    if (updatingData) {
      float emgvalue = readEMGValue();
      server.send(200, "text/plain", String(emgvalue, 6)); // Precisão de 6 casas decimais
    }
  }
}