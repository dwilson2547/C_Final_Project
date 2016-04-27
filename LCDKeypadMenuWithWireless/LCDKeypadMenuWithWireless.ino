#include <LiquidCrystal.h>
#include <SPI.h>
#include "RF24.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// RADIO STUFF
RF24 radio(2,3); 
byte addresses[][6] = {"1Node","2Node"};

// Structures, Holy SHIT
struct dataStruct {
  int currentTemp; 
  int setTemp; 
  int fan; 
  int power; 
  int mode; 
} txData;

//States for the menu.
int currentMenuItem = 1;
int lastState = 1;

int edit = 0; 
// CURRENT TEMP VARS
int currentTemp; 
// TEMP VARS
int temp = 72;
// FAN VARS
const int fanLOW = 1; 
const int fanMED = 2; 
const int fanHI = 3; 
int fan = fanLOW; 
// POWER VARS
const int powerON = 1; 
const int powerOFF = 0; 
int power = powerOFF;
// MODE VARS
const int mHEAT = 1; 
const int mCOOL = 2; 
const int mAUTO = 3; 
const int mWEATHER = 4; 
int mode = mAUTO; 

void setup() {
  
  //Set the characters and column numbers.
  lcd.begin(16, 2);
  showCurrentTemp(); 

  radio.begin(); 
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setAutoAck(1);
  radio.setChannel(108); 
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  radio.powerUp();
  
  Serial.begin(115200); 
}
 
void loop() {
  updateTemp(); 
  mainMenu();
  transmit();
}
void transmit() {
  radio.stopListening();

  // Setup Struct for Transfer
  txData.currentTemp = currentTemp; 
  txData.setTemp = temp; 
  txData.fan = fan; 
  txData.power = power; 
  txData.mode = mode;

  //Serial.println("Now Sending"); 

  if (!radio.write(&txData, sizeof(txData))) {
    //Serial.println("Transmit failed"); 
  }
}
int checkButton() {
  int x = analogRead(0); 
  int state = 0; 
  if (x < 100) {
    //Right
    state = 4; 
  } else if (x < 200) {
   //Up
    state = 1;
    delay(200);
  } else if (x < 400){
   //Down
    state = 2;
  } else if (x < 600){
    //Left
    state = 5; 
  } else if (x < 800){
    //Select
    state = 3;
  }
  return state; 
}

void mainMenu() {
  int state = 0;
  state = checkButton(); 
  //If we are out of bounds on the menu then reset it.
  
   if (state != lastState) {
    switch (state) {
      case 1: 
        currentMenuItem -= 1;
        if (currentMenuItem < 1) {
          currentMenuItem = 5; 
        } 
        displayMenu(currentMenuItem); 
        break;
      case 2: 
        currentMenuItem += 1; 
        if (currentMenuItem > 5) {
          currentMenuItem = 1; 
        }
        displayMenu(currentMenuItem); 
        break;
      case 3: 
        selectMenu(currentMenuItem); 
        break;
      case 4: 
        if (edit == 1) {
          itemUp(currentMenuItem); 
        }
        break; 
      case 5:
        if (edit == 1) {
          itemDown(currentMenuItem);  
        }
        break;
    }
      lastState = state;
   }
  delay(5);
}
// INCRIMENT and DECRIMENT FUNCTIONS
void itemUp(int n) {
  switch(n) {
    case 1: 
      showTemp(0); 
      break;
    case 2: 
      temp += 1; 
      showSetTemp(); 
      break;
    case 3: 
      if (power == powerON) {
        power = powerOFF;
      }
      else {
        power = powerON; 
      }
      showPower();
      break;
    case 4: 
      if (fan == 3) {
        fan = 1; 
      }
      else {
        fan += 1;
      }
      showFan();
      break;
    case 5: 
      if (mode == 4) {
        mode = 1; 
      }
      else {
        mode += 1; 
      }
      showMode();
      break;
  }
}
void itemDown(int n) {
  switch(n) {
    case 1: 
      showTemp(1);
      break;
    case 2: 
      temp -= 1; 
      showSetTemp();
      break;
    case 3: 
      if (power == powerON) {
        power = powerOFF;
      }
      else {
        power = powerON; 
      }
      showPower();
      break;
    case 4: 
      if (fan == 1) {
        fan = 3; 
      }
      else {
        fan -= 1; 
      }
      showFan();
      break;
    case 5: 
      if (mode == 1) {
        mode = 4; 
      }
      else {
        mode -= 1; 
      }
      showMode();
      break;
  }
}
//Display Menu Option based on Index.
void displayMenu(int x) {
    edit = 0;
     switch (x) {
      case 1:
        showCurrentTemp(); 
        break;
      case 2:
        showCurrentSetTemp();
        break;
       case 3:
        showCurrentPower(); 
        break;
      case 4:
        showCurrentFan(); 
        break;
      case 5: 
        showCurrentMode();
        break;
    }
}
void selectMenu(int x) {
  edit = 1; 
  switch (x) { 
    case 1: 
      showTemp(0); 
      break;
    case 2:
      showSetTemp(); 
      break; 
    case 3: 
      showPower();
      break;
    case 4: 
      showFan(); 
      break;
    case 5: 
      showMode(); 
      break;
  }
}
// UPDATE CURRENT TEMP
void updateTemp() {
  float tempT = (5.0 * analogRead(A1)*100.0)/1024;
  Serial.print("Temp: ");
  tempT = (tempT * (9.0/5.0) + 32);
  currentTemp = tempT; 
  Serial.println(currentTemp); 
}
// DISPLAY METHODS
void showTemp(int n) {
  int tC = (currentTemp - 32) * 5.0/9.0;
  lcd.clear();
  lcd.setCursor(0,0);
  if (n == 0) {
    lcd.print("Fahrenheit");
    lcd.setCursor(0,1);
    lcd.print(currentTemp);
  }
  else {
    lcd.print("Celcius");
    lcd.setCursor(0,1);
    lcd.print(tC); 
  }
}
void showSetTemp() {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("Change Set Temp"); 
  lcd.setCursor(0,1); 
  lcd.print(temp); 
}
void showFan() {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("Change Fan"); 
  lcd.setCursor(0,1); 
  if (fan == fanLOW) {
    lcd.print("Low"); 
  }
  else if (fan == fanMED) {
    lcd.print("Medium"); 
  }
  else {
    lcd.print("High"); 
  }
}
void showPower() {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("Change Power"); 
  lcd.setCursor(0,1); 
  if (power == powerON) {
    lcd.print("On");
  }
  else {
    lcd.print("Off");
  }
}
void showMode() {
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Change Mode"); 
  lcd.setCursor(0,1); 
  switch (mode) {
    case 1: 
      lcd.print("Heat"); 
      break;
    case 2: 
      lcd.print("Cool");
      break;
    case 3: 
      lcd.print("Auto"); 
      break;
    case 4:
      lcd.print("Weather"); 
      break;
  }
}
// SHOW CURRENT VALS
void showCurrentTemp() {
  int currentTemp1 = (5.0 * analogRead(A1)*100.0)/1024;
  int currentTemp2 = (5.0 * analogRead(A2)*100.0)/1024; 
  currentTemp = (5.0 * analogRead(A2)*100.0)/1024;
  currentTemp = (currentTemp * (9.0/5.0) + 32); 
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("-> Current Temp"); 
  lcd.setCursor(0,1); 
  lcd.print(currentTemp); 
}
void showCurrentPower(){
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("-> Power"); 
  lcd.setCursor(0,1); 
  if (power == powerON) {
    lcd.print("On");
  }
  else {
    lcd.print("Off");
  }
}
void showCurrentSetTemp() {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("-> SetP Temp"); 
  lcd.setCursor(0,1); 
  lcd.print(temp); 
}
void showCurrentMode() {
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("-> Mode"); 
  lcd.setCursor(0,1); 
  switch (mode) {
    case 1: 
      lcd.print("Heat"); 
      break;
    case 2: 
      lcd.print("Cool");
      break;
    case 3: 
      lcd.print("Auto"); 
      break;
    case 4:
      lcd.print("Weather"); 
      break;
  }
}
void showCurrentFan() {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("-> Fan"); 
  lcd.setCursor(0,1); 
  if (fan == fanLOW) {
    lcd.print("Low"); 
  }
  else if (fan == fanMED) {
    lcd.print("Medium"); 
  }
  else {
    lcd.print("High"); 
  }
}
