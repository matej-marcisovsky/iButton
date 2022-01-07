#include <OneWire.h>

#define BUFFER_SIZE 20
#define BUTTON_PIN 8
#define BUZZER_FREQUENCY 4000
#define BUZZER_PIN 7
#define PROBE_PIN 6

OneWire ibutton(PROBE_PIN);

byte buttonData[BUFFER_SIZE];
byte controlButtonData[BUFFER_SIZE];
int currentMode = LOW;
bool buttonWritten = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("iButton");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != currentMode) {
    tone(BUZZER_PIN, BUZZER_FREQUENCY, 300);
    currentMode = buttonState;
  }

  if (currentMode == LOW) { // Read Mode
      if (readButton(buttonData) && readButton(controlButtonData) && verify()) {
        Serial.println("Reading...");
        printID();
        tone(BUZZER_PIN, BUZZER_FREQUENCY, 150);
        delay(1500);
      } else {
        ibutton.reset_search();
        delay(200);

        return;
      }
  }

  if (currentMode == HIGH) { // Write Mode
    if (!ibutton.reset()) {
      buttonWritten = false;
    } else if (!buttonWritten) {
      Serial.println("Writing...");
      printID();
      writeButton();

      if (readButton(controlButtonData) && verify()) {
        buttonWritten = true;

        Serial.println("Done!");
        tone(BUZZER_PIN, BUZZER_FREQUENCY, 150);
        delay(1000);
      }
    }
  }
}

void printID() {
  Serial.print("ID: ");

  for (int x = 0; x < 8; x++) {
    Serial.print(buttonData[x], HEX); // Print the buffer content in LSB. For MSB: for (int x = 8; x > 0; x--).
  }

  Serial.println("");
}

bool verify() {
  return memcmp(buttonData, controlButtonData, sizeof(buttonData)) == 0;
}

bool readButton(byte* data) {
  if (ibutton.search(data)) {
    ibutton.reset_search();
    return true;
  }

  return false;
}

void writeButton() {
  ibutton.write(0xD5);

  for (int x = 0; x < 8; x++) {
    writeByte(buttonData[x]);
    Serial.print('-');
  }

  Serial.println("");
}

void writeByte(byte data) {
  for(int x = 0; x < 8; x++) {
    if (data & 1) {
      digitalWrite(PROBE_PIN, LOW); pinMode(PROBE_PIN, OUTPUT);
      delayMicroseconds(60);
      pinMode(PROBE_PIN, INPUT); digitalWrite(PROBE_PIN, HIGH);
      delay(10);
    } else {
      digitalWrite(PROBE_PIN, LOW); pinMode(PROBE_PIN, OUTPUT);
      pinMode(PROBE_PIN, INPUT); digitalWrite(PROBE_PIN, HIGH);
      delay(10);
    }

    data = data >> 1;
  }
}
