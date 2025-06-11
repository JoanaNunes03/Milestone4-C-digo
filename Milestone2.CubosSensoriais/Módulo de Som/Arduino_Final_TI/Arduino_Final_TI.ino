int ledVerde = 2;
int ledVermelho = 3;

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int cor = random(0, 2); // 0 ou 1

  if (cor == 0) {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
    Serial.println("VERDE");
  } else {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, HIGH);
    Serial.println("VERMELHO");
  }

  delay(3000); // Espera 3 segundos antes de trocar
}