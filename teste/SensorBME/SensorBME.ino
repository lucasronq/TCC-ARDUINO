#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

const char* ssid = "GABINETE";
const char* password = "cosmorama2025";

#define SDA_PIN 4
#define SCL_PIN 3
#define BME280_ADDRESS 0x76

Adafruit_BME280 bme;
WebServer server(80);

String gerarJSON() {
  StaticJsonDocument<256> doc;
  doc["sensor"] = "BME280";
  doc["temperatura_c"] = bme.readTemperature();
  doc["pressao_hpa"] = bme.readPressure() / 100.0F;
  doc["umidade_%"] = bme.readHumidity();
  doc["altitude_m"] = bme.readAltitude(1013.25);
  doc["ip"] = WiFi.localIP().toString();

  String json;
  serializeJson(doc, json);
  return json;
}

void handleJSON() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", gerarJSON());
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Conectado ao Wi-Fi!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Erro: BME280 não detectado!");
    while (1);
  }

  server.on("/json", handleJSON);
  server.begin();
  Serial.println("Servidor de dados iniciado!");
}

void loop() {
  server.handleClient();
}
