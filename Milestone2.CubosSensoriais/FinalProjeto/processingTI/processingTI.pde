import processing.serial.*;

Serial myPort;
String currentColorName = "";
color currentColor;
String feedbackMessage = "";
int feedbackTime = 0;
int feedbackDuration = 1000;

void setup() {
  size(400, 250);

  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.bufferUntil('\n');

  textAlign(CENTER, CENTER);
  textSize(24);
}

void draw() {
  background(255);
  noStroke();
  if (currentColorName != "") {
    fill(0);
    text(currentColorName, width / 2, 40);
    fill(currentColor);
    rect(width / 2 - 50, 80, 100, 100);
  }

  if (feedbackMessage != "" && millis() - feedbackTime < feedbackDuration) {
    fill(0);
    textSize(20);
    text(feedbackMessage, width / 2, height - 40);
    textSize(24); // reset in case
  }
}

void speak(String message) {
  try {
    String command = "PowerShell -Command \"Add-Type –AssemblyName System.Speech; " +
                     "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; " +
                     "$speak.Speak('" + message + "');\"";
    Runtime.getRuntime().exec(command);
  } catch (Exception e) {
    println("Error speaking: " + e.getMessage());
  }
}

void serialEvent(Serial p) {
  String inData = p.readStringUntil('\n');
  if (inData != null) {
    inData = trim(inData);

    if (inData.equals("CLEAR")) {
      currentColorName = "";
      currentColor = color(0);
      return;
    }

    if (inData.equals("Red") || inData.equals("Green") || inData.equals("Blue") || inData.equals("Yellow")) {
      currentColorName = inData;

      if (currentColorName.equals("Red")) {
        currentColor = color(255, 0, 0);
      } else if (currentColorName.equals("Green")) {
        currentColor = color(0, 255, 0);
      } else if (currentColorName.equals("Blue")) {
        currentColor = color(0, 0, 255);
      } else if (currentColorName.equals("Yellow")) {
        currentColor = color(255, 255, 0);
      }
      
      speak(currentColorName);
      return;
    }

    if (inData.equals("Well done!") || inData.equals("Wrong button!")|| inData.equals("Wrong sensor!")) {
      feedbackMessage = inData;
      feedbackTime = millis();
        speak(feedbackMessage);
    }
  }
}
