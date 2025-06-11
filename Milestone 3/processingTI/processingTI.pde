import processing.serial.*;

Serial myPortSensor;
Serial myPortBotoes;

String modoAtual = "sensores"; // controla o modo atual do programa: "sensores" ou "botoes"
int acertosSensores = 0;
int acertosBotoes = 0;

// variáveis para exibir a cor atual recebida pelos sensores
String currentColorName = "";
color currentColor = color(0);

// variáveis para exibir a cor atual dos botões
String corBotaoAtual = "";
color corBotao = color(0);

// mensagens de feedback (positivo ou negativo)
String feedbackMessage = "";
int feedbackTime = 0;
int feedbackDuration = 1600; // tempo que a mensagem de feedback fica visível

// controla os comandos agendados (usado para transições entre modos)
boolean aguardarProximoComando = false;
int tempoAgendado = 0;
String comandoAgendado = "";
Serial portaAgendada = null;

void setup() {
  size(400, 300);

  myPortSensor = new Serial(this, Serial.list()[2], 9600);
  myPortBotoes = new Serial(this, Serial.list()[3], 9600);

  myPortSensor.bufferUntil('\n');
  myPortBotoes.bufferUntil('\n');

  textAlign(CENTER, CENTER);
  textSize(24);
}

void draw() {
  background(255);

  // mostra o modo atual
  fill(0);
  textSize(18);
  text("Modo atual: " + modoAtual.toUpperCase(), width / 2, 20);

  // modo de sensores (mostra a cor lida)
  if (modoAtual.equals("sensores")) {
    text("Sensor Color:", width/2, 50);
    if (!currentColorName.equals("")) {
      fill(currentColor);
      rect(width/2 - 60, 70, 120, 60);
      fill(0);
      text(currentColorName, width/2, 140);
    }
    text("Acertos sensores: " + acertosSensores, width / 2, 180);

    // modo de botões (mostra o botão pressionado)
  } else {
    text("Botão Cor Atual:", width/2, 50);
    if (!corBotaoAtual.equals("")) {
      fill(corBotao);
      rect(width/2 - 60, 70, 120, 60);
      fill(0);
      text(corBotaoAtual, width/2, 140);
    }
    text("Acertos botões: " + acertosBotoes, width / 2, 180);
  }

  // exibe mensagem de feedback (por exemplo: "Well done!" ou "Wrong button!")
  if (!feedbackMessage.equals("") && millis() - feedbackTime < feedbackDuration) {
    fill(0);
    textSize(20);
    text(feedbackMessage, width/2, height - 30);
  }

  // envia comandos agendados após um pequeno atraso
  if (aguardarProximoComando && millis() >= tempoAgendado) {
    if (portaAgendada != null && comandoAgendado != "") {
      portaAgendada.write(comandoAgendado + "\n");
      println("Comando enviado: " + comandoAgendado);
    }
    aguardarProximoComando = false;
  }
}

void speak(String message) {
  try {
    String command = "PowerShell -Command \"Add-Type -AssemblyName System.Speech; " +
      "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; " +
      "$speak.Speak('" + message + "');\"";
    Runtime.getRuntime().exec(command);
    delay(500);
  }
  catch (Exception e) {
    println("Erro ao falar: " + e.getMessage());
  }
}

// função chamada automaticamente quando chega um dado pela porta serial
void serialEvent(Serial p) {
  String inData = p.readStringUntil('\n');
  if (inData != null) {
    inData = trim(inData);
    println("Color:" + inData);

    // caso seja do sensor e este modo esteja ativo
    if (p == myPortSensor && modoAtual.equals("sensores")) {
      if (inData.equals("CLEAR")) {
        currentColorName = "";
        currentColor = color(0);
        return;
      }

      // se receber uma cor válida
      if (inData.equals("Orange") || inData.equals("Pink") || inData.equals("Purple") || inData.equals("Light Blue")) {
        currentColorName = inData;
        currentColor = getSensorColor(inData);
        speak(currentColorName);

        // se acertou
      } else if (inData.equals("Well done!")) {
        acertosSensores++;
        feedbackMessage = inData;
        feedbackTime = millis() + 1500;
        speak("Well done!");

        delay(1000);

        // troca para modo "botoes" se atingir número de acertos
        if (acertosSensores >= 4) {
          speak("Now the buttons!");
          currentColorName = "";
          modoAtual = "botoes";
          acertosSensores = 0;
          delay(600);  // só para garantir que a porta está pronta
          aguardarProximoComando = true;
          tempoAgendado = millis() + 1500; // 1.5 segundos de delay
          comandoAgendado = "NEXT";
          portaAgendada = myPortBotoes;
        }

        // se errou
      } else if (inData.equals("Wrong sensor!")) {
        feedbackMessage = inData;
        feedbackTime = millis();
        speak("Wrong sensor!");
      }

      // caso seja da porta dos botões
    } else if (p == myPortBotoes && modoAtual.equals("botoes")) {
      if (inData.startsWith("COR:")) {
        corBotaoAtual = inData.substring(4);
        corBotao = getBotaoColor(corBotaoAtual);
        speak(corBotaoAtual);

        // se acertou
      } else if (inData.equals("Well done!")) {
        acertosBotoes++;
        feedbackMessage = inData;
        feedbackTime = millis() + 1500;
        speak("Well done!");

        delay(1000);

        // volta ao modo sensores após acertos suficientes
        if (acertosBotoes >= 6) {
          speak("Now the sensors!");
          corBotaoAtual = "";
          modoAtual = "sensores";
          acertosBotoes = 0;
          delay(600);
          aguardarProximoComando = true;
          tempoAgendado = millis() + 1500;
          comandoAgendado = "NEXT";
          portaAgendada = myPortSensor;
        }

        // se errou
      } else if (inData.equals("Wrong button!")) {
        feedbackMessage = inData;
        feedbackTime = millis();
        speak("Wrong button!");
      }
    }
  }
}

// função para converter nome da cor dos sensores para valor RGB
color getSensorColor(String nome) {
  if (nome.equals("Orange")) return color(255, 165, 0);
  if (nome.equals("Pink")) return color(255, 105, 180);
  if (nome.equals("Purple")) return color(128, 0, 255);
  if (nome.equals("Light Blue")) return color(50, 50, 205);
  return color(128); // Cor padrão caso nome seja inválido
}

// função para converter nome da cor dos botões para valor RGB
color getBotaoColor(String nome) {
  if (nome.equals("Yellow")) return color(255, 255, 0);
  if (nome.equals("Green")) return color(0, 255, 0);
  if (nome.equals("Blue")) return color(0, 0, 255);
  if (nome.equals("Red")) return color(255, 0, 0);
  return color(128);
}
