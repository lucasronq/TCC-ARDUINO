#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin("GABINETE", "cosmorama2025");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  server.on("/dados", []() {
    StaticJsonDocument<200> doc;
    doc["temperatura"] = 23.5;
    doc["umidade"] = 18;
    doc["status"] = "ok";

    String json;
    serializeJson(doc, json);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
