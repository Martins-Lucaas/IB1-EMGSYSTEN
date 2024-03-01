#include <WiFi.h>
#include <WebServer.h>

<<<<<<< Updated upstream
const char *ssid = "Net";
const char *password = "12345678";
=======
hw_timer_t *My_timer = NULL;

const char *ssid = "Martins WiFi6";
const char *password = "17031998";
>>>>>>> Stashed changes

WebServer server(80);

bool updatingData = false; // Variável para controlar a atualização dos dados

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
          "background-color: #f2f2f2;"
        "}"
        ".container {"
          "max-width: 1500px;"
          "margin: 50px auto;" // Centraliza o conteúdo na página
          "padding: 20px;"
          "background-color: #fff;"
          "border-radius: 10px;"
          "box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
          "text-align: center;"
        "}"
        ".container h2 {"
          "font-size: 24px;"
          "color: #333;"
        "}"
        ".container p {"
          "font-size: 18px;"
          "color: #666;"
        "}"
        ".container canvas {"
          "width: 100%;" // Permite que o gráfico se expanda horizontalmente
          "height: 480px;"
        "}"
        ".container table {"
          "margin-top: 20px;"
          "border-collapse: collapse;"
          "width: auto;"
          "overflow-x: auto;" // Adiciona rolagem horizontal caso o conteúdo exceda a largura disponível
          "margin-left: auto;" // Centraliza a tabela na página
          "margin-right: auto;" // Centraliza a tabela na página
        "}"
        ".container th, .container td {"
          "border: 1px solid #dddddd;"
          "text-align: left;"
          "padding: 8px;"
          "min-width: 100px;" // Define uma largura mínima para cada célula da tabela
        "}"
        ".container th {"
          "background-color: #f2f2f2;"
        "}"
        ".button {"
          "border-radius: 20px;"
          "padding: 10px 20px;"
          "margin: 10px;"
          "cursor: pointer;"
          "border: none;"
          "color: white;"
          "font-size: 16px;"
          "transition: background-color 0.2s ease;"
        "}"
        ".button.start[disabled] {"
          "background-color: #ccc;"
          "cursor: not-allowed;"
        "}"
        ".button.start {"
          "background-color: green;"
        "}"
        ".button.start:hover {"
          "background-color: darkgreen;"
        "}"
        ".button.start:active {"
          "background-color: forestgreen;"
        "}"
        ".button.stop {"
          "background-color: red;"
        "}"
        ".button.stop:hover {"
          "background-color: darkred;"
        "}"
        ".button.stop:active {"
          "background-color: firebrick;"
        "}"
        ".button.save {"
          "background-color: blue;"
        "}"
        ".button.save:hover {"
          "background-color: darkblue;"
        "}"
        ".button.save:active {"
          "background-color: royalblue;"
        "}"
        ".button.table {"
          "background-color: orange;"
        "}"
        ".button.table:hover {"
          "background-color: darkorange;"
        "}"
        ".button.table:active {"
          "background-color: orangered;"
        "}"
        ".hidden {"
          "display: none;"
        "}"
      "</style>"
    "</head>"
    "<body>"
      "<div class='container'>"
        "<h2>Projeto IB1</h2>"
        "<p>Valor do Potenciometro: <span id='pot_value'>0</span></p>"
        "<canvas id='potChart' width='1280' height='720'></canvas>"
        "<button id='startButton' class='button start' onclick='start()'>Iniciar</button>"
        "<button class='button stop' onclick='stop()'>Parar</button>"
        "<button class='button save' onclick='save()'>Salvar</button>"
        "<button id='showTableButton' class='button table' onclick='showTable()'>Ver Tabela</button>"
        "<div id='tableContainer' class='hidden'></div>"
      "</div>"
      "<script>"
        "var ctx = document.getElementById('potChart').getContext('2d');"
        "var potChart = new Chart(ctx, {"
          "type: 'line',"
          "data: {"
            "labels: [],"
            "datasets: [{"
              "label: 'Valor do Potenciometro',"
              "data: [],"
              "fill: false,"
              "borderColor: 'rgb(75, 192, 192)',"
            "}]"
          "},"
          "options: {"
            "responsive: true,"
            "scales: {"
              "x: {"
                "type: 'linear',"
                "position: 'bottom'"
              "},"
              "y: {"
                "suggestedMin: 0,"
                "suggestedMax: 4095"
              "}"
            "}"
          "}"
        "});"
        "var timeElapsed = 0;"
        "var intervalId;"
        "function updatePotChart(potValue) {"
<<<<<<< Updated upstream
          "timeElapsed += 2;" // Incrementa o tempo decorrido por 0.2 (200 ms)
          "potChart.data.labels.push(timeElapsed);" // Adiciona o tempo decorrido aos rótulos do eixo X
          "potChart.data.datasets[0].data.push(potValue);" // Adiciona o valor do potenciômetro aos dados do gráfico
          "potChart.update();" // Atualiza o gráfico
        "}"
        "function start() {"
          "intervalId = setInterval(function() {"
            "var xhttp = new XMLHttpRequest();"
            "xhttp.onreadystatechange = function() {"
              "if (this.readyState == 4 && this.status == 10) {"
                "document.getElementById('pot_value').innerHTML = this.responseText;"
                "updatePotChart(parseInt(this.responseText));" // Atualiza o gráfico com o valor do potenciômetro
              "}"
            "};"
            "xhttp.open('GET', '/potValue', true);" // Envia uma solicitação para o servidor para obter o valor do potenciômetro
            "xhttp.send();"
          "}, 1);" // Iniciar atualização periódica a cada 200 ms
          "document.getElementById('startButton').disabled = true;" // Desabilita o botão 'Iniciar'
=======
          "timeElapsed += 0.2;"
          "potChart.data.labels.push(timeElapsed.toFixed(1));"
          "potChart.data.datasets[0].data.push(potValue);"
          "var chart = potChart;"
          "var maxX = Math.max.apply(Math, chart.data.labels);"
          "var minX = Math.min.apply(Math, chart.data.labels);"
          "if (timeElapsed >= maxX) {"
            "chart.options.scales.x.max = timeElapsed + 5;"
            "chart.options.scales.x.min = minX;"
            "chart.update();"
          "} else {"
            "potChart.update();"
          "}"
          "var tableHTML = '<table><tr><th>Tempo (s)</th><th>Valor do Potenciometro</th></tr>';"
          "for (var i = 0; i < potChart.data.labels.length; i++) {"
            "tableHTML += '<tr><td>' + potChart.data.labels[i] + '</td><td>' + potChart.data.datasets[0].data[i] + '</td></tr>';"
          "}"
          "tableHTML += '</table>';"
          "document.getElementById('tableContainer').innerHTML = tableHTML;"
        "}"
        "function start() {"
          "if (!updatingData) {"
            "intervalId = setInterval(function() {"
              "var xhttp = new XMLHttpRequest();"
              "xhttp.onreadystatechange = function() {"
                "if (this.readyState == 4 && this.status == 200) {"
                  "document.getElementById('pot_value').innerHTML = this.responseText;"
                  "updatePotChart(parseInt(this.responseText));"
                "}"
              "};"
              "xhttp.open('GET', '/potValue', true);"
              "xhttp.send();"
            "}, 10);"
            "document.getElementById('startButton').disabled = true;"
            "updatingData = true;"
          "}"
>>>>>>> Stashed changes
        "}"
        "function stop() {"
          "clearInterval(intervalId);"
          "document.getElementById('startButton').disabled = false;"
          "updatingData = false;"
        "}"
        "function save() {"
          "alert('Dados salvos!');"
        "}"
        "function showTable() {"
          "var tableContainer = document.getElementById('tableContainer');"
          "if (tableContainer.classList.contains('hidden')) {"
            "tableContainer.classList.remove('hidden');"
            "document.getElementById('showTableButton').innerText = 'Ocultar Tabela';"
            "stop();"
          "} else {"
            "tableContainer.classList.add('hidden');"
            "document.getElementById('showTableButton').innerText = 'Ver Tabela';"
          "}"
        "}"
      "</script>"
    "</body>"
  "</html>";
<<<<<<< Updated upstream
  server.send(10, "text/html", html);
=======

  server.send(200, "text/html", html);
>>>>>>> Stashed changes
}

void setup() {
  Serial.begin(115200);

  // Conectar-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  // Impressão do endereço IP após a conexão bem sucedida
  Serial.println("Conectado à rede WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Definir as rotas
  server.on("/", HTTP_GET, handleRoot);
  server.on("/potValue", HTTP_GET, []() {server.send(10, "text/plain", String(analogRead(34)));
  });

  // Iniciar o servidor
  server.begin();

  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient(); // Lidar com as solicitações dos clientes
}