#include "pitches.h"

#define blueLed 11
#define greenLed 10
#define yellowLed 9
#define redLed 6

#define blueBtn 12
#define greenBtn 2
#define yellowBtn 3
#define redBtn 4

#define LEN 4
#define MAX_NUM_OF_LIGHTS 3
#define BuzzTone 5

#define START 31
#define GAME_ON 32
#define WIN 33
#define LOSE 34

const int btnsArry[LEN] = { blueBtn, greenBtn, yellowBtn, redBtn };
const int ledsArry[LEN] = { blueLed, greenLed, yellowLed, redLed };
const int tones[LEN] = { NOTE_C4, NOTE_G3, NOTE_A3, NOTE_F2 };

int val[LEN];
int lastVal[LEN];
unsigned long lastPressTime[LEN];
int ChosenIndexes[MAX_NUM_OF_LIGHTS];
int btnsPressedIndexes[MAX_NUM_OF_LIGHTS];

unsigned long pressStart;
unsigned long pressEnd;
int corrPress = 0;
int cntPress = 0;
int currState;
bool restart = false;

void setup() {
  for (int i = 0; i < LEN; i++) {
    pinMode(ledsArry[i], OUTPUT);
    pinMode(btnsArry[i], INPUT_PULLUP);
  }
  pinMode(BuzzTone, OUTPUT);
  randomSeed(analogRead(A1));
  currState = START;
}

void loop() {
  switch (currState) {
    case START: StartGame(); break;
    case GAME_ON: GameOn(); break;
    case WIN: Win(); break;
    case LOSE: Lose(); break;
  }
}

void ChooseRandomLights() {
  int rndNum;
  for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
    rndNum = random(0, LEN);
    if (!valInArry(rndNum)) {
      ChosenIndexes[i] = rndNum;
    } else {
      i--;
    }
  }
}

void Btn_Led_Setup() {
  for (int i = 0; i < LEN; i++) {
    lastVal[i] = digitalRead(btnsArry[i]);
    lastPressTime[i] = millis();
    LedOff(i);
  }
}

int GetPressedBtn(bool restart) {
  int BtnPressed = -1;
  for (int i = 0; i < LEN; i++) {
    val[i] = digitalRead(btnsArry[i]);
    if ((val[i] == LOW) && (lastVal[i] == HIGH) && (millis() - lastPressTime[i] > 150)) {
      lastPressTime[i] = millis();
      BtnPressed = i;
      if (!restart) {
        btnsPressedIndexes[cntPress] = i;
        cntPress++;
      }
    }
    lastVal[i] = val[i];
  }
  return BtnPressed;
}

void LedOn(int chnl) {
  digitalWrite(ledsArry[chnl], HIGH);
}
void LedOff(int chnl) {
  digitalWrite(ledsArry[chnl], LOW);
}

void ShowLights() {
  for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
    LedOn(ChosenIndexes[i]);
  }
  delay(1000);
  for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
    LedOff(ChosenIndexes[i]);
  }
  delay(500);
  tone(BuzzTone, tones[0], 300);
}

bool valInArry(int num) {
  for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
    if (ChosenIndexes[i] == num) {
      return true;
    }
  }
  return false;
}

void StartGame() {
  noTone(BuzzTone);
  Btn_Led_Setup();
  for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
    ChosenIndexes[i] = -1;
    btnsPressedIndexes[i] = -1;
  }
  ChooseRandomLights();
  if (restart) {
    delay(200);
  }
  ShowLights();
  corrPress = 0;
  cntPress = 0;
  currState = GAME_ON;
}

void GameOn() {
  int btn = GetPressedBtn(false);
  if (btn != -1) {
    if (cntPress == 1) {
      pressStart = millis();
    }
  }
  if (cntPress == 3) {
    pressEnd = millis();
    if (btnsPressedIndexes[0] != btnsPressedIndexes[1] || btnsPressedIndexes[0] != btnsPressedIndexes[2] || btnsPressedIndexes[1] != btnsPressedIndexes[2]) {
      int i = 0;
      while (i < cntPress) {
        corrPress += valInArry(btnsPressedIndexes[i++]);
      }
      if (corrPress == 3 && pressEnd - pressStart <= 1000) {
        currState = WIN;
      }
      if (corrPress != 3 || pressEnd - pressStart > 1000) {
        currState = LOSE;
      }
    } else {
      currState = LOSE;
    }
  }
}

void Win() {
  restart = true;
  LedOn(1);
  tone(BuzzTone, tones[1]);
  if (GetPressedBtn(restart) != -1) {
    currState = START;
  }
}

void Lose() {
  restart = true;
  LedOn(3);
  tone(BuzzTone, tones[2]);
  if (GetPressedBtn(restart) != -1) {
    currState = START;
  }
}
