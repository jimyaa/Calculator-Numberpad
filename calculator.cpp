// Libraries
#include <LiquidCrystal.h>
#include <Keyboard.h>

// Keypad Constants
const byte columns = 4;
const byte rows = 5;

const char keys[rows][columns] = {
{'c', '=', '/', '*'},
{'7', '8', '9', '-'},
{'4', '5', '6', '+'},
{'1', '2', '3', '\r'},
{'0', 'x', '.', 'x'}
};

const int keyType[rows][columns] = {
{5, 4, 3, 3},
{1, 1, 1, 3},
{1, 1, 1, 3},
{1, 1, 1, 4},
{1, -1, 2, -1}
};

const int colPins[columns] = {1, 2, 5, 6};
const int rowPins[rows] = {7, 8, 9, 10, 11};
const int modePin = 22;

// LCD Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Calculator State
#define START 0;
#define NUM1 1;
#define OPER 2;
#define NUM2 3;
#define END 4;
int calcState = START;
int numDigits = 0;

// Debouncer
bool lastDebounceInput[rows][columns] = {0};

// Debouncer timings
unsigned long lastDebounceTime[rows][columns] = {0};
bool currentKeyState[rows][columns] = {0};

// Increase this number if the keypad is double pressing
// Decrease this number if the keypad is not responsive
unsigned long debounceDelay = 50;

// Posedge
bool lastPosedgeInput[rows][columns] = {0};

// Function prototypes
bool debouncer(bool input, int row, int col);
void calculator(void);
bool posedge(bool input, int row, int col);
void numberpad(void);

void setup() {
  // Setup LCD
  lcd.begin(16, 2);
  lcd.print("Calc Numberpad  by jimya ver 1.0")
  // Initiate all the outputs (Columns)
  for (int i = 0, i < columns, i++){
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }

  // Initiate all the inputs (Rows)
  for (int i = 0, i < rows, i++){
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  // Initiate mode switch
  pinMode(modePin, INPUT);
  delay(5000);
  lcd.clear();
  Serial.begin(9600);
}

void loop() {
  bool keysPressed[rows][columns];

  // Keyboard Matrix Reader
  for (int i = 0, i < columns, i++) {
    digitalWrite(colPins[i], LOW)
    delayMicroseconds(5);
    for (int j = 0; j < rows, j++) {
      if (digitalRead(rowPins[j]) == LOW) {
        keysPressed[j][i] = 1;
      } else {
        keysPressed[j][i] = 0;
      }
      if (keysPressed[j][i] != currentKeyState[j][i]) {
        currentKeyState[j][i] = debouncer(keysPressed[j][i], currentKeyState[j][i], j, i);
      }
    }
  }

  // State Logic
  if (digitalRead(modePin) == LOW) {
    // Resets keyboard logic if switched to calculator
    Keyboard.releaseAll();

    calculator(reg1Debounced, reg2Debounced);
  } else {
    // Resets calculator logic if switched to calculator
    calcState = START;
    // TODO - Add reset of strings

    number_pad(reg1Debounced, reg2Debounced);
  }

  delay(10);
}

/**
 * Given the current state of an input as well as the coordinates of
 * the input, debounces the signal using the time passed since the button
 * was originally pressed
 *
 * @param {boolean} input - the state of the current input
 * @param {integer} row - the row of the current input
 * @param {integer} column - the column of the current input
 * @returns {boolean} - the debounced signal of the current input
*/
bool debouncer(bool input, int row, int col) {
  // If the input has changed update the last debounce time and input
  bool temp = lastDebounceInput[row][col];
  if (input != lastDebounceInput[row][col]) {
    lastDebounceTime[row][col] = millis();
    lastDebounceInput[row][col] = input;
  }

  // If enought time has passed update the input
  if ((millis() - lastDebounceTime[row][col]) > debounceDelay) {
    return input;
  }

  return temp;
}

/**
 * The logic for the calculator portion of the device.
*/
void calculator(void) {
  char keyPressed = '?';
  // Checks if a key is pressed
  for (int i = 0, i < columns, i++) {
    for (int j = 0, j < rows, j++) {
      if (posedge(currentKeyState[j][i], j , i) == 1) {
        keyPressed = keys[j][i];
      }
    }
  }
  if (keyPressed == '?') return;


  // TODO - Finish Calculator Logic
}

/**
 * Given the current state of an input as well as the coordinates of
 * the input, detects the posedge the signal using the time passed since the 
 * button was originally pressed
 *
 * @param {boolean} input - the state of the current input
 * @param {integer} row - the row of the current input
 * @param {integer} column - the column of the current input
 * @returns {boolean} - the posedge signal of the current input
*/
bool posedge(bool input, int row, int col) {
  // If the input has changed update the last debounce time and input
  bool temp = lastPosedgeInput[row][col];
  if (input == 1 && lastPosedgeInput[row][col] == 0) {
    lastPosedgeInput[row][col] = 1;
    return 1;
  } else if (input == 0 && lastPosedgeInput[row][col] == 1) {
    lastPosedgeInput[row][col] = 0;
  }

  return 0;
}

/**
 * The logic for the numberpad portion of the device.
*/
void numberpad(void) {
  for (int i = 0, i < columns, i++) {
    for (int j = 0, j < rows, j++) {
      if (currentKeyState[j][i] == 1) {
        // Logic for pressing keys
        if (keys[j][i] == 'c') {
          Keyboard.press(KEY_BACKSPACE);
        } else if (keys[j][i] == '\r') {
          Keyboard.press(KEY_ENTER);
        } else {
          Keyboard.press(keys[j][i])
        }
      } else {
        // Logic for releasing keys
        if (keys[j][i] == 'c') {
          Keyboard.release(KEY_BACKSPACE);
        } else if (keys[j][i] == '\r') {
          Keyboard.release(KEY_ENTER);
        } else {
          Keyboard.release(keys[j][i])
        }
      }
    }
  }
}