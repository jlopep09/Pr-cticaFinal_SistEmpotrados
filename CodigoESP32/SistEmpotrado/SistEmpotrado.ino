#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include "MQ135.h"
#include <Wire.h>
//Librerias usadas(algunas no se usan pero las tengo instaladas por si acaso)
//NTPClient - Fabrice Weinberg V3.2.1
//Adafruit BMP085 Library - Adafruit v1.2.4
//Adafruit BusIO - Adafruit v1.16.3
//Adafruit Unified Sensor - Adafruit 1.1.15
//AsyncTCP - dvarrel 1.1.4
//DHT sensor library- Adafruit v1.4.6
//ESP Async WebServer - Me-No-Dev v3.6.0
//ESP Async TCP - dvarrel v1.2.4
//MQ135 GreorgK - ViliusKraujutis v1.1.1
//MQUnifiedsensor - Miguel Califa v3.0.0
//Time - Michael Morgolis v1.6.1

// Define los pines SDA y SCL si usas GPIO1 y GPIO2
#define SDA_PIN 1
#define SCL_PIN 2

//NETWORK CONFIG
const char* ssid = "jlopep06"; //wifiname 
const char* password = "jlopep06"; //wifipass
const char* serverName = "http://192.168.174.240:8000/data/sensordata"; //fastapi endpoint 

//INTERVAL CONFIG
int send_interval = 60000; //1 min = 60000

//INPUT PIN
#define PHOTO_PIN 4
#define DHTPIN 10 

//BMP180 CONFIG
Adafruit_BMP085 bmp;

//MQ135 CONFIG
const int ANALOGPIN = 7;
MQ135 gasSensor = MQ135(ANALOGPIN);

//EXTRA DHT CONFIG
#define DHTTYPE DHT11    
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

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bmp.begin()) {
    Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
  }

  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int lightLevel = analogRead(PHOTO_PIN);
    float pressure = bmp.readPressure()/1000.0; //unidad en KPa
    int ppm = gasSensor.getPPM()/100000; //Algunos usuarios recomiendan dividir el resultado entre 100000 creo que para usar el CO2

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

  delay(send_interval);
}
