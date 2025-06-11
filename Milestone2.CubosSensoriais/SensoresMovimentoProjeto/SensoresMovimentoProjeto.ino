#define echoPinL 7   // Echo sensor Direito
#define trigPinL 8   // Trigger sensor direito
#define echoPinA 6   // Echo sensor esquerdo
#define trigPinA 9   // Trigger sensor esquerdo

#define LEDPinV 13   // LED verde (acerto)
#define LEDPinVm 12  // LED vermelho (erro)

long durationL, durationA;
float distanceL, distanceA;

int corAlvo = 0; // 1 = Azul (esquerdo), 2 = Laranja (direito)
bool aguardandoTentativa = true;

void setup() {
  Serial.begin(9600);

  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);

  pinMode(trigPinA, OUTPUT);
  pinMode(echoPinA, INPUT);

  pinMode(LEDPinV, OUTPUT);
  pinMode(LEDPinVm, OUTPUT);

  randomSeed(analogRead(0)); // Para gerar números aleatórios

  novaRodada();
}

void loop() {
  // Ler os dois sensores
  distanceL = lerDistancia(trigPinL, echoPinL);
  distanceA = lerDistancia(trigPinA, echoPinA);

  // Verificações
  if (aguardandoTentativa) {
    if (distanceL > 0 && distanceL < 20) {
      // Passou no sensor esquerdo
      if (corAlvo == 2) {
        acerto();
      } else {
        erro();
      }
    } else if (distanceA > 0 && distanceA < 20) {
      // Passou no sensor direito
      if (corAlvo == 1) {
        acerto();
      } else {
        erro();
      }
    }
  }

  delay(100);
}

float lerDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH, 100000);
  float dist = (duracao * 0.034) / 2;
  return dist;
}

void novaRodada() {
  // Aleatorio entre 1 (azul) e 2 (laranja)
  corAlvo = random(1, 3); // 1 ou 2
  aguardandoTentativa = true;

  if (corAlvo == 1) {
    Serial.println("Cor sorteada: AZUL (sensor esquerdo)");
  } else {
    Serial.println("Cor sorteada: LARANJA (sensor direito)");
  }

  digitalWrite(LEDPinV, LOW);
  digitalWrite(LEDPinVm, LOW);
}

void acerto() {
  Serial.println("ACERTOU!");
  digitalWrite(LEDPinV, HIGH);
  digitalWrite(LEDPinVm, LOW);
  aguardandoTentativa = false;
  delay(1000);
  novaRodada();
}

void erro() {
  Serial.println("ERROU!");
  digitalWrite(LEDPinVm, HIGH);
  digitalWrite(LEDPinV, LOW);
  aguardandoTentativa = false;
  delay(1000);
  novaRodada();
}
