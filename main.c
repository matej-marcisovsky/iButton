#include <OneWire.h>

#define I_BUTTON_PIN 5
#define BUTTON_PIN 4

OneWire ibutton(I_BUTTON_PIN);

byte buffer[20]; // Array to store the iButton ID.
int buttonState = 0;
bool buttonWritten = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("iButton");
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    if (!ibutton.search(buffer)) {
      ibutton.reset_search();
      delay(200);

      return;
    } else {
      Serial.println("Reading...");
      printID();
      delay(1000);
    }
  }

  if (buttonState == HIGH && !ibutton.reset()) {
    buttonWritten = false;
  }

  if (buttonState == HIGH && ibutton.reset() && !buttonWritten) {
    buttonWritten = true;
    Serial.println("Writing...");
    printID();

    ibutton.write(0xD5);

    for (int x = 0; x < 8; x++) {
      writeByte(buffer[x]);
      Serial.print('-');
    }

    Serial.println("");
    Serial.println("Done!");
    delay(1000);
  }
}

void printID() {
  Serial.print("ID: ");

  for (int x = 0; x < 8; x++) {
    Serial.print(buffer[x], HEX); // Print the buffer content in LSB. For MSB: for (int x = 8; x>0; x--).
  }

  Serial.println("");
}

void writeByte(byte data) {
  for(int x = 0; x < 8; x++) {
    if (data & 1) {
      digitalWrite(I_BUTTON_PIN, LOW); pinMode(I_BUTTON_PIN, OUTPUT);
      delayMicroseconds(60);
      pinMode(I_BUTTON_PIN, INPUT); digitalWrite(I_BUTTON_PIN, HIGH);
      delay(10);
    } else {
      digitalWrite(I_BUTTON_PIN, LOW); pinMode(I_BUTTON_PIN, OUTPUT);
      pinMode(I_BUTTON_PIN, INPUT); digitalWrite(I_BUTTON_PIN, HIGH);
      delay(10);
    }

    data = data >> 1;
  }
}
