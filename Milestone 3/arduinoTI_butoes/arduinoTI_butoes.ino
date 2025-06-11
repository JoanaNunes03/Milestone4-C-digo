#include <Adafruit_NeoPixel.h>

#define PIN 6        // pino digital ligado aos LEDs
#define NUM_LEDS 64  // número total de LEDs na tira

// cria o objeto "strip" para controlar os LEDs, com configuração GRB e frequência de 800kHz
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

const int botaoPins[4] = { 2, 3, 4, 5 };
const uint32_t cores[4] = {
  strip.Color(255, 255, 0),  // Yellow
  strip.Color(0, 255, 0),    // Green
  strip.Color(0, 0, 255),    // Blue
  strip.Color(255, 0, 0)     // Red
};
const char* nomes[4] = { "Yellow", "Green", "Blue", "Red" };

// índices para armazenar a cor atual e a cor anterior
int corAtual = -1;
int corAnterior = -1;

String input = "";

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(30);
  strip.show();

  for (int i = 0; i < 4; i++) {
    pinMode(botaoPins[i], INPUT_PULLUP);
  }

  randomSeed(analogRead(A0));
  novaCor();  // escolhe a primeira cor aleatória
}

void loop() {
  receberComando();

  // verifica se algum botão foi pressionado
  for (int i = 0; i < 4; i++) {
    if (digitalRead(botaoPins[i]) == LOW) { // botão pressionado (nível LOW)
      if (i == corAtual) {
        Serial.println("Well done!");
        novaCor();
      } else {
        Serial.println("Wrong button!");
      }
      delay(300);
    }
  }
}

void novaCor() {
  int nova;

  // garante que a nova cor seja diferente da anterior
  do {
    nova = random(0, 4);
  } while (nova == corAnterior);

  corAnterior = corAtual;
  corAtual = nova;

// atualiza todos os LEDs com a nova cor
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, cores[corAtual]);
  }
  strip.show();

  Serial.print("COR:");
  Serial.println(nomes[corAtual]);
}

void receberComando() {
  // lê os caracteres recebidos pela Serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { // fim de linha indica fim do comando
      input.trim();

// executa comandos recebidos
      if (input.equals("OFF")) {
        apagar();
      } else if (input.equals("NEXT")) {
        novaCor();  // mostra nova cor e envia nome via Serial
      }

      input = "";  // limpa após processar comando
    } else {
      input += c; // adiciona caractere ao comando
    }
  }
}

void apagar() {
  // apaga todos os LEDs (define a cor como 0 = preto)
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show(); //atualiza os LEDs
}
