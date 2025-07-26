#define NUM_LEDS 8
#define NUM_COLUMNS 120
#define FONT_WIDTH 5
#define LETTER_SPACING 1

const int ledPins[NUM_LEDS] = {2, 3, 4, 5, 6, 7, 8, 9};
const int hallPin = 10;

volatile bool hallTriggered = false;
unsigned long lastTriggerTime = 0;
unsigned long rotationTime = 20000;

const char* message = "Hackclub is cool ";
byte font[][5] = {
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
  {0x00, 0x00, 0x00, 0x00, 0x00}  // SPACE (last index)
};

byte pattern[NUM_COLUMNS][NUM_LEDS];

void IRAM_ATTR hallInterrupt() {
  hallTriggered = true;
}

void setupPattern() {
  int col = 0;
  while (*message && col < NUM_COLUMNS - 6) {
    char ch = toupper(*message);
    byte* letter;

    if (ch >= 'A' && ch <= 'Z') {
      letter = font[ch - 'A'];
    } else {
      letter = font[26]; // space
    }

    for (int i = 0; i < FONT_WIDTH && col < NUM_COLUMNS; i++) {
      for (int row = 0; row < NUM_LEDS; row++) {
        pattern[col][row] = (letter[i] >> row) & 0x01;
      }
      col++;
    }

    // Add spacing
    for (int s = 0; s < LETTER_SPACING && col < NUM_COLUMNS; s++) {
      for (int r = 0; r < NUM_LEDS; r++) {
        pattern[col][r] = 0;
      }
      col++;
    }

    message++;
  }
}

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(hallPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallPin), hallInterrupt, FALLING);
  setupPattern();
}

void loop() {
  if (hallTriggered) {
    hallTriggered = false;
    unsigned long now = micros();
    rotationTime = now - lastTriggerTime;
    lastTriggerTime = now;

    for (int i = 0; i < NUM_COLUMNS; i++) {
      showColumn(i);
      delayMicroseconds(rotationTime / NUM_COLUMNS);
    }
  }
}

void showColumn(int col) {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], pattern[col][i]);
  }
}
