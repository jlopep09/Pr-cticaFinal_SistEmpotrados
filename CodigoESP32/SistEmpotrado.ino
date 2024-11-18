#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

//NETWORK CONFIG
const char* ssid = "wifiName";
const char* password = "wifiPass";
const char* serverName = "http://localhost:apiport/apiruteexample";

//INTERVAL CONFIG
int send_interval = 10000;

//INPUT PIN
#define PHOTO_PIN 32
#define DHTPIN 19 


//EXTRA DHT CONFIG
#define DHTTYPE DHT22    
DHT dht(DHTPIN, DHTTYPE);


void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi!");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int lightLevel = analogRead(PHOTO_PIN);


    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el sensor DHT22");
      return;
    }

    // Crear el JSON
    String postData = "{\"temperature\": " + String(temperature) + 
                      ", \"humidity\": " + String(humidity) + 
                      ", \"light_level\": " + String(lightLevel) + "}";

    // Inicia la conexión al endpoint FastAPI
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Envía el POST y recibe la respuesta
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.println("Error en la solicitud: " + String(httpResponseCode));
    }
    http.end();

  } else {
    Serial.println("Reconectando a WiFi...");
    connectToWiFi();
  }

  delay(send_interval); // Cada minuto (ajustable)
}
