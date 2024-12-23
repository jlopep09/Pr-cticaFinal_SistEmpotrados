#include <WiFi.h>
#include <HTTPClient.h>
//#include <DHT.h>
//#include <Adafruit_BMP085.h>
//#include "MQ135.h"

//NETWORK CONFIG
const char* ssid = "jlopep09";
const char* password = "jlopep09";
const char* serverName = "http://192.168.140.240:8000/data/sensordata";

//INTERVAL CONFIG
int send_interval = 10000;

//INPUT PIN
#define PHOTO_PIN 32
#define DHTPIN 19 

//BMP180 CONFIG
//Adafruit_BMP085 bmp;

//MQ135 CONFIG
const int ANALOGPIN = 13;
//MQ135 gasSensor = MQ135(ANALOGPIN);

//EXTRA DHT CONFIG
#define DHTTYPE DHT11    
//DHT dht(DHTPIN, DHTTYPE);


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
  //dht.begin();

  //if (!bmp.begin()) {
  ////  Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
  //}

  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    float temperature = 10;//dht.readTemperature();
    float humidity = 10;//dht.readHumidity();
    int lightLevel = 10;//analogRead(PHOTO_PIN);
    int pressure = 10;//bmp.readPressure();
    float ppm = 10;//gasSensor.getPPM();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el sensor DHT11");
      return;
    }

    // Crear el JSON
    String postData = "{\"temperature\": " + String(temperature) + 
                      ", \"humidity\": " + String(humidity) + 
                      ", \"light_level\": " + String(lightLevel) + 
                      ", \"pressure\": " + String(pressure) +
                      ", \"ppm\": " + String(ppm) +
                      "}";

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
