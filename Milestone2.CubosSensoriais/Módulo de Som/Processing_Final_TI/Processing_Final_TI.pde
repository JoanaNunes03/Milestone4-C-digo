import processing.serial.*;

Serial porta;
String cor = "";

void setup() {
  println(Serial.list());
  porta = new Serial(this, Serial.list()[2], 9600);
  //porta.bufferUntil('\n');
}

void draw() {
  // nada aqui
}

void serialEvent(Serial porta) {
  cor = porta.readStringUntil('\n');
  if (cor != null) {
    cor = cor.trim();
    println("Recebido: " + cor);
    falar(cor);
  }
}

void falar(String texto) {
  String comando = "";
  
  // Para Windows
  if (System.getProperty("os.name").toLowerCase().contains("win")) {
    comando = "cmd.exe /c PowerShell -Command \"Add-Type –AssemblyName System.Speech; (New-Object System.Speech.Synthesis.SpeechSynthesizer).Speak('" + texto + "');\"";
  }
  
  try {
    Runtime.getRuntime().exec(comando);
  } catch (Exception e) {
    e.printStackTrace();
  }
}
