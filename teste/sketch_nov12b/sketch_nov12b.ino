#define SENSOR_PIN 1      // Pino analógico do sensor (A0 ou S)
#define BOTAO_PIN 2       // Pino de botão opcional (GND quando pressionado)

int valorSeco = 3500;     // Valor padrão para solo seco (será calibrado)
int valorMolhado = 1200;  // Valor padrão para solo úmido (será calibrado)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Calibração Automática do Sensor de Umidade do Solo ===");
  Serial.println("Coloque o sensor no solo SECO e pressione 's' no Serial ou o botão.");
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP);
}

void loop() {
  // Verifica se o botão foi pressionado ou 's' digitado no Serial
  if (digitalRead(BOTAO_PIN) == LOW) {
    calibrarSeco();
  } else if (Serial.available()) {
    char comando = Serial.read();
    if (comando == 's') calibrarSeco();
    if (comando == 'm') calibrarMolhado();
  }

  // Lê o valor atual do sensor
  int leitura = analogRead(SENSOR_PIN);
  float tensao = leitura * (3.3 / 4095.0);

  // Calcula a umidade com base nos valores calibrados
  int umidade = map(leitura, valorSeco, valorMolhado, 0, 100);
  umidade = constrain(umidade, 0, 100);

  // Mostra as informações no Serial
  Serial.print("Leitura: ");
  Serial.print(leitura);
  Serial.print(" | Tensão: ");
  Serial.print(tensao, 2);
  Serial.print(" V | Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");

  delay(1000);
}

// ---------------------------------------------------------
// Função para calibrar o valor de solo seco
void calibrarSeco() {
  Serial.println(">> Calibrando SOLO SECO...");
  delay(2000);
  valorSeco = analogRead(SENSOR_PIN);
  Serial.print("Valor seco calibrado: ");
  Serial.println(valorSeco);
  Serial.println("Agora coloque o sensor em solo úmido e pressione 'm' no Serial ou o botão novamente.");
  delay(2000);
}

// Função para calibrar o valor de solo molhado
void calibrarMolhado() {
  Serial.println(">> Calibrando SOLO MOLHADO...");
  delay(2000);
  valorMolhado = analogRead(SENSOR_PIN);
  Serial.print("Valor molhado calibrado: ");
  Serial.println(valorMolhado);
  Serial.println("Calibração completa! Valores armazenados na RAM.");
  delay(2000);
}
