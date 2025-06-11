#include <FastLED.h>

const int width = 16;
const int height = 16;
const int numLeds = width * height;
const int ledMatrix = 3;

int button1 = 6;
int button2 = 7;
int button3 = 8;
int button4 = 9;


CRGB leds[numLeds];

const char * corNomes[] = { "Red", "Green", "Blue", "Yellow" };
const CRGB corVals[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow };
const int numCores = sizeof(corNomes) / sizeof(corNomes[0]);

bool quadAtivo[4] = { false, false, false, false };
CRGB quadCores[4];
int quadCerto = -1;

int XY(int x, int y) {
  if (y % 2 == 0) {
    return y * width + x;
  } else {
    return y * width + (width - 1 - x);
  }
}

const int trig1 = 11;
const int echo1 = 10;
const int trig2 = 13;
const int echo2 = 12;

long lerDist(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

void fillQuad(int q, CRGB cor) {
  int xStart = (q % 2) * 8;
  int yStart = (q / 2) * 8;
  for (int y = yStart; y < yStart + 8; y++) {
    for (int x = xStart; x < xStart + 8; x++) {
      leds[XY(x, y)] = cor;
    }
  }
}

void clearMatrix() {
  fill_solid(leds, numLeds, CRGB::Black);
}

void randomQuads() {
   clearMatrix();
  Serial.println("CLEAR");
  FastLED.show();
  delay(1000);  

  for (int i = 0; i < 4; i++) {
    quadAtivo[i] = false;
    quadCores[i] = CRGB::Black;
  }

  int numQuads = random(2, 5);
  int chosenQuadrants[4] = { 0, 1, 2, 3 };
  int availableColors[numCores];
  for (int i = 0; i < numCores; i++) availableColors[i] = i;

  for (int i = 0; i < 4; i++) {
    int r = random(i, 4);
    int temp = chosenQuadrants[i];
    chosenQuadrants[i] = chosenQuadrants[r];
    chosenQuadrants[r] = temp;
  }

  for (int i = 0; i < numCores; i++) {
    int r = random(i, numCores);
    int temp = availableColors[i];
    availableColors[i] = availableColors[r];
    availableColors[r] = temp;
  }

  for (int i = 0; i < numQuads; i++) {
    int q = chosenQuadrants[i];
    int indexCor = availableColors[i];
    CRGB cor = corVals[indexCor];
    fillQuad(q, cor);
    quadAtivo[q] = true;
    quadCores[q] = cor;
  }

  FastLED.show();

  int activeQs[4];
  int count = 0;
  for (int i = 0; i < 4; i++) {
    if (quadAtivo[i]) {
      activeQs[count++] = i;
    }
  }

  quadCerto = activeQs[random(0, count)];
  CRGB targetCor = quadCores[quadCerto];
  for (int i = 0; i < numCores; i++) {
    if (targetCor == corVals[i]) {
      Serial.println(corNomes[i]);
      break;
    }
  }
}


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, ledMatrix, GRB>(leds, numLeds);
  FastLED.clear();
  FastLED.show();

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);

  pinMode(trig1, OUTPUT);
pinMode(echo1, INPUT);
pinMode(trig2, OUTPUT);
pinMode(echo2, INPUT);

  randomSeed(analogRead(A0));
  FastLED.setBrightness(1);
  randomQuads();
}

bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};

void loop() {
  int buttons[4] = {button1, button2, button3, button4};

  for (int i = 0; i < 4; i++) {
    bool currentState = digitalRead(buttons[i]);

    if (lastButtonState[i] == LOW && currentState == HIGH) {
      if (quadCerto == i) {
        Serial.println("Well done!");
        randomQuads();
      } else {
        Serial.println("Wrong button!");
      }
    }

    lastButtonState[i] = currentState;
  }

  // Distance-based "buttons"
long dist1 = lerDist(trig1, echo1);
long dist2 = lerDist(trig2, echo2);

const int triggerDistance = 15;

static bool wasClose1 = false;
static bool wasClose2 = false;

bool isClose1 = dist1 > 0 && dist1 < triggerDistance;
bool isClose2 = dist2 > 0 && dist2 < triggerDistance;

if (wasClose1 && !isClose1) {
  if (quadCerto == 0) {
    Serial.println("Well done!");
    randomQuads();
  } else {
    Serial.println("Wrong sensor!");
  }
}
if (wasClose2 && !isClose2) {
  if (quadCerto == 2) {
    Serial.println("Well done!");
    randomQuads();
  } else {
    Serial.println("Wrong sensor!");
  }
}

wasClose1 = isClose1;
wasClose2 = isClose2;


  delay(100);
}

