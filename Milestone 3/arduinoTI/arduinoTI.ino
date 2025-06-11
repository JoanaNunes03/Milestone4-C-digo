#include <Adafruit_NeoPixel.h>

#define DATA_PIN 3
#define LEDS_PER_MATRIX 64 // número de LEDs por matriz (ex: 8x8)
#define NUM_MATRICES 4 // total de matrizes ligadas
#define TOTAL_LEDS (LEDS_PER_MATRIX * NUM_MATRICES) // LEDs totais no sistema

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// arrays com os pinos dos sensores ultrassónicos
const int trigPins[NUM_MATRICES] = { 4, 6, 8, 10 };
const int echoPins[NUM_MATRICES] = { 5, 7, 9, 11 };

// cores fixas associadas a cada matriz
uint32_t coresFixas[NUM_MATRICES] = {
  strip.Color(255, 140, 0),   // Laranja
  strip.Color(255, 20, 147),  // Rosa
  strip.Color(128, 0, 255),   // Roxo
  strip.Color(50, 50, 205)    // Azul Claro
};

const char* nomesFixos[NUM_MATRICES] = {
  "Orange", "Pink", "Purple", "Light Blue"
};

// guarda a ordem atual e anterior das cores/matrizes
int ordemAtual[NUM_MATRICES] = { 0, 1, 2, 3 };
int ordemAnterior[NUM_MATRICES] = { -1, -1, -1, -1 };

// indica se cada matriz ainda está ativa
bool matrizAtiva[NUM_MATRICES] = { true, true, true, true };

// guarda a sequência aleatória de cores que o jogador deve seguir
int filaCores[NUM_MATRICES];

// índice atual na sequência de cores (qual cor o jogador deve acertar agora)
int corAlvoIdx = 0;

String input = "";

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(30);
  strip.show();

  // configura os pinos dos sensores
  for (int i = 0; i < NUM_MATRICES; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  // inicializa o gerador de números aleatórios
  randomSeed(analogRead(A0));

  // gera uma nova sequência de exibição
  novaSequencia();
  novaFilaDeCores();
  atualizarMatrizes();
  anunciarProximaCor();
}

void loop() {
  receberComandos();  // verifica se há comandos pela Serial

  // para cada matriz ativa, verifica a distância
  for (int i = 0; i < NUM_MATRICES; i++) {
    if (!matrizAtiva[i]) continue;

    long dist = lerDistancia(trigPins[i], echoPins[i]);

    // se a distância for pequena (sensor detectou a mão, por exemplo)
    if (dist > 0 && dist < 15) {
      int corEsperadaIdx = filaCores[corAlvoIdx]; // índice da cor correta

      if (i == corEsperadaIdx) {
        // acertou a cor/matriz correta
        apagarMatriz(i);
        matrizAtiva[i] = false;
        strip.show();

        Serial.println("Well done!");
        Serial.println("CLEAR");
        corAlvoIdx++;

        if (corAlvoIdx < NUM_MATRICES) {
          delay(800); //pausa antes da próxima cor
          anunciarProximaCor();
        } else {
          // completou todas as cores corretamente
          delay(1500);
          novaSequencia();
          novaFilaDeCores();
          atualizarMatrizes();
          anunciarProximaCor();
        }
      } else {
        Serial.println("Wrong sensor!");
      }

      delay(500); // tempo para evitar leituras repetidas
    }
  }
}

void receberComandos() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      input.trim();

      if (input.equals("OFF")) {
        apagarTodasMatrizes();
        strip.show();
      } else if (input.equals("NEXT")) {
        anunciarProximaCor();
      }

      input = "";  // limpa o buffer após processar
    } else {
      input += c;
    }
  }
}

// gera nova ordem aleatória de matrizes (sem repetir a anterior)
void novaSequencia() {
  int nova[NUM_MATRICES] = { 0, 1, 2, 3 };
  bool repetida;

  do {
    // embaralha o array nova[]
    for (int i = 0; i < NUM_MATRICES; i++) {
      int r = random(i, NUM_MATRICES);
      int temp = nova[i];
      nova[i] = nova[r];
      nova[r] = temp;
    }

    // verifica se é igual à anterior
    repetida = true;
    for (int i = 0; i < NUM_MATRICES; i++) {
      if (nova[i] != ordemAnterior[i]) {
        repetida = false;
        break;
      }
    }
  } while (repetida);

  // atualiza ordem e reativa todas as matrizes
  for (int i = 0; i < NUM_MATRICES; i++) {
    ordemAnterior[i] = ordemAtual[i];
    ordemAtual[i] = nova[i];
    matrizAtiva[i] = true;
  }
}

// cria fila de cores (ordem aleatória que deve acertar)
void novaFilaDeCores() {
  for (int i = 0; i < NUM_MATRICES; i++) {
    filaCores[i] = i;
  }

  // embaralha filaCores[]
  for (int i = 0; i < NUM_MATRICES; i++) {
    int r = random(i, NUM_MATRICES);
    int temp = filaCores[i];
    filaCores[i] = filaCores[r];
    filaCores[r] = temp;
  }

  corAlvoIdx = 0;  // começa do início
}

// envia pela Serial o nome da próxima cor a ser acertada
void anunciarProximaCor() {
  if (corAlvoIdx < NUM_MATRICES) {
    int corAlvo = ordemAtual[filaCores[corAlvoIdx]];
    Serial.println(nomesFixos[corAlvo]);
  }
}

// acende todos os LEDs de uma matriz específica com uma cor
void acenderMatriz(int idx, uint32_t cor) {
  int start = idx * LEDS_PER_MATRIX;
  for (int i = start; i < start + LEDS_PER_MATRIX; i++) {
    strip.setPixelColor(i, cor);
  }
}

// apaga todos os LEDs de uma matriz específica
void apagarMatriz(int idx) {
  int start = idx * LEDS_PER_MATRIX;
  for (int i = start; i < start + LEDS_PER_MATRIX; i++) {
    strip.setPixelColor(i, 0);
  }
}

// apaga todas as matrizes
void apagarTodasMatrizes() {
  for (int i = 0; i < NUM_MATRICES; i++) {
    apagarMatriz(i);
  }
}

// atualiza os LEDs de todas as matrizes com as novas cores
void atualizarMatrizes() {
  for (int i = 0; i < NUM_MATRICES; i++) {
    int corIndex = ordemAtual[i];
    acenderMatriz(i, coresFixas[corIndex]);
  }
  strip.show();
}

// lê a distância de um sensor ultrassónico (em cm)
long lerDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duracao = pulseIn(echo, HIGH, 30000); // timeout de 30 ms
  return duracao * 0.034 / 2; // conversão para centímetros
}
