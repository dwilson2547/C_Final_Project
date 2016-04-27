#include <SPI.h>
#include "RF24.h"
#include <Servo.h>
#include <Wire.h>

RF24 radio(9,10); 
byte addresses[][6] = {"1Node","2Node"};

// Structures, Holy SHIT
struct dataStruct {
  int currentTemp;
  int setTemp;
  int fan;
  int power;
  int mode;
} txData, oldData;

// VARS
// FAN VARS
const int fanLOW = 165; 
const int fanMED = 130; 
const int fanHI = 85; 
// POWER VARS
const int powerON = 125; 
const int powerOFF = 75; 
// TEMP VARS
const int tempHI = 10; 
const int tempLOW = 60; 
// RECEIVED TEMP 
int receivedTemp;
// SERVOS
Servo tempServo; 
Servo powerServo; 
Servo fanServo; 

void setup() {
  Serial.begin(9600); 

  radio.begin(); 
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setAutoAck(1);
  radio.setChannel(108); 
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  radio.powerUp();

  pinMode(6, OUTPUT); 
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);

  tempServo.attach(3); 
  powerServo.attach(5);  
  fanServo.attach(6);

  Wire.begin(); 
}
void loop() {
  receiveWireless();
  if (txData.power == 1){
    setMode();
  }
  if (oldData.power != txData.power) {
    changePower();
  }
  //if (oldData.fan != txData.fan) {
    changeFan(); 
 // }
  oldData = txData;
  i2cTransmit();  
  serialRead(); 
}

int receive; 
void serialRead() {
  while (Serial.available() > 0) {
    receivedTemp = (int)Serial.read(); 
  }
}
void i2cTransmit() {
  //Serial.println("Sending"); 
  Wire.beginTransmission(8); 
  Wire.write(txData.currentTemp);
  Wire.write(",");
  Wire.write(txData.setTemp); 
  Wire.endTransmission(); 
}
void changeFan() {
  switch (txData.fan) {
    case 1:
      //Serial.println("Fan 1"); 
      fanServo.write(fanLOW); 
      break;
    case 2:
      //Serial.println("Fan 2"); 
      fanServo.write(fanMED);
      break;
    case 3: 
      //Serial.println("Fan 3"); 
      fanServo.write(fanHI); 
      break;
  }
}
void changePower() {
  if (txData.power == 0) {
    setPower(powerOFF);
    //Serial.println("Power off"); 
  }
  else {
    setPower(powerON);
    //Serial.println("Power on"); 
  }
}
void setMode() {
  //Serial.println("SetMode");
  switch(txData.mode) {
    case 1:
      //Serial.println("Mode 1"); 
      if (txData.currentTemp < txData.setTemp + 3) {
        setPower(powerON); 
        setTemp(tempHI); 
      }
      else {
        setPower(powerOFF); 
      }
      break;
    case 2:
      //Serial.println("Mode 2"); 
      if (txData.currentTemp > txData.setTemp - 3) {
        setPower(powerON); 
        setTemp(tempLOW); 
       }
      else {
        setPower(powerOFF); 
      }
      break;
    case 3:
      //Serial.println("Mode 3"); 
      if (txData.currentTemp > txData.setTemp + 3) {
        setPower(powerON); 
        setTemp(tempLOW); 
      }
      else if (txData.currentTemp < txData.setTemp - 3) {
        setPower(powerON); 
        setTemp(tempHI); 
      }
      else 
      {
        setPower(powerOFF); 
      } 
      break;
    case 4:
      //Serial.println("Mode 4"); 
      if (txData.currentTemp < receivedTemp + 3) {
        setPower(powerON); 
        setTemp(tempHI); 
      }
      else if (txData.currentTemp > receivedTemp - 3) {
        setPower(powerON); 
        setTemp(tempLOW); 
      }
      else 
      {
        setPower(powerOFF); 
      }
      break;
  }
}
void heatMode() {
  if (txData.currentTemp < txData.setTemp + 3) {
    setPower(powerON); 
    setTemp(tempHI); 
  }
  else {
    setPower(powerOFF); 
  }
}
void coolMode() {
  if (txData.currentTemp > txData.setTemp - 3) {
    setPower(powerON); 
    setTemp(tempLOW); 
  }
  else {
    setPower(powerOFF); 
  }
}
void autoMode() {
  if (txData.currentTemp > txData.setTemp + 3) {
    setPower(powerON); 
    setTemp(tempLOW); 
  }
  else if (txData.currentTemp < txData.setTemp - 3) {
    setPower(powerON); 
    setTemp(tempHI); 
  }
  else 
  {
    setPower(powerOFF); 
  }
}
void weatherMode() {
  if (txData.currentTemp < receivedTemp + 3) {
    setPower(powerON); 
    setTemp(tempHI); 
  }
  else if (txData.currentTemp > receivedTemp - 3) {
    setPower(powerON); 
    setTemp(tempLOW); 
  }
  else 
  {
    setPower(powerOFF); 
  }
}
void setPower(int n) {
  powerServo.write(n); 
}
void setFan(int n) {
  fanServo.write(n); 
}
void setTemp(int n) {
  tempServo.write(n); 
}
void receiveWireless() {
  if (radio.available())
  {
    //Serial.println("Radio available"); 
    while (radio.available()) 
    {
      radio.read( &txData, sizeof(txData)); 
    }
    /*
    Serial.println("Data: "); 
    Serial.print("Current Temp: "); 
    Serial.println(txData.currentTemp); 
    Serial.print("Set Temp: "); 
    Serial.println(txData.setTemp);
    Serial.print("Fan Speed: "); 
    Serial.println(txData.fan); 
    Serial.print("Power: "); 
    Serial.println(txData.power); 
    Serial.print("Mode: "); 
    Serial.println(txData.mode); 


    Serial.println("Data: "); 
    Serial.print("Current Temp: "); 
    Serial.println(oldData.currentTemp); 
    Serial.print("Set Temp: "); 
    Serial.println(oldData.setTemp);
    Serial.print("Fan Speed: "); 
    Serial.println(oldData.fan); 
    Serial.print("Power: "); 
    Serial.println(oldData.power); 
    Serial.print("Mode: "); 
    Serial.println(oldData.mode); 
    */
  }
  delay(1000); 
}

