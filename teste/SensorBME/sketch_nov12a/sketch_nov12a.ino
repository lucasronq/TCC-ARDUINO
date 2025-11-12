#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>  // Biblioteca para montar JSON

// ===== CONFIGURAÇÃO DO WIFI =====
const char* ssid = "GABINETE";
const char* password = "cosmorama2025";

// ===== CONFIGURAÇÃO DO BME280 =====
#define SDA_PIN 4
#define SCL_PIN 3
#define BME280_ADDRESS 0x76

Adafruit_BME280 bme;

// ===== CONFIGURAÇÃO =====
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Conectando ao Wi-Fi...");

  // --- Conexão Wi-Fi ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");

  // --- Inicializa o sensor BME280 ---
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("❌ Erro: BME280 não detectado!");
    Serial.println("Verifique a fiação ou tente o endereço 0x77.");
    while (1);
  }
  Serial.println("✅ BME280 detectado com sucesso!");
}

void loop() {
  // --- Leitura dos dados do sensor ---
  float temperatura = bme.readTemperature();
  float pressao = bme.readPressure() / 100.0F; // em hPa
  float umidade = bme.readHumidity();
  float altitude = bme.readAltitude(1013.25);  // pressão ao nível do mar

  // --- Monta o JSON ---
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["sensor"] = "BME280";
  jsonDoc["temperatura_c"] = temperatura;
  jsonDoc["pressao_hpa"] = pressao;
  jsonDoc["umidade_%"] = umidade;
  jsonDoc["altitude_m"] = altitude;
  jsonDoc["wifi_ip"] = WiFi.localIP().toString();

  // --- Converte o JSON para string ---
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // --- Exibe no Serial ---
  Serial.println(jsonString);
  Serial.println("----------------------------------");

  delay(3000); // lê a cada 3 segundos
}
