#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>          // 🔴 OBRIGATÓRIO NO ESP32-C3
#include <ArduinoOTA.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

// ===== WIFI =====
const char* ssid = "GABINETE";
const char* password = "cosmorama2025";

// ===== IP FIXO =====
IPAddress local_IP(192,168,100,9);
IPAddress gateway(192,168,100,1);
IPAddress subnet(255,255,255,0);

// ===== BME280 =====
#define SDA_PIN 4
#define SCL_PIN 3
#define BME280_ADDRESS 0x76

Adafruit_BME280 bme;
WebServer server(80);

// ===== ENDPOINT JSON =====
void handleDados() {
  StaticJsonDocument<256> json;

  json["sensor"] = "BME280";
  json["temperatura_c"] = bme.readTemperature();
  json["pressao_hpa"]   = bme.readPressure() / 100.0;
  json["umidade_%"]     = bme.readHumidity();
  json["altitude_m"]    = bme.readAltitude(1013.25);
  json["wifi_ip"]       = WiFi.localIP().toString();

  String resposta;
  serializeJson(json, resposta);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", resposta);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // ===== WIFI =====
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Conectando Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.println(WiFi.localIP());

  // ===== MDNS (ESSENCIAL PARA OTA NO C3) =====
  if (!MDNS.begin("esp32-bme280")) {
    Serial.println("Erro ao iniciar mDNS");
    while (1);
  }

  Serial.println("mDNS iniciado: esp32-bme280.local");

  // ===== I2C =====
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("❌ BME280 não encontrado!");
    while (1);
  }

  Serial.println("✅ BME280 iniciado!");

  // ===== OTA =====
  ArduinoOTA.setHostname("esp32-bme280");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA iniciado");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA finalizado");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro OTA [%u]\n", error);
  });

  ArduinoOTA.begin();
  Serial.println("OTA pronto!");

  // ===== SERVER =====
  server.on("/dados", handleDados);
  server.begin();
}

void loop() {
  ArduinoOTA.handle();   // 🔴 ESSENCIAL
  server.handleClient();
}
