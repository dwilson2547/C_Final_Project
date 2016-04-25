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
const int fanLOW = 1; 
const int fanMED = 2; 
const int fanHI = 3; 
// POWER VARS
const int powerON = 1; 
const int powerOFF = 2; 
// TEMP VARS
const int tempHI = 1; 
const int tempLOW = 2; 
// RECEIVED TEMP 
int receivedTemp; 
// SERVOS
Servo tempServo; 
Servo powerServo; 
Servo fanServo; 
char[] = "<li>" + txData.currentTemp + "</li><li>"
void setup() {
  Serial.begin(115200); 

  radio.begin(); 
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setAutoAck(1);
  radio.setChannel(108); 
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  radio.powerUp();

  tempServo.attach(3); 
  powerServo.attach(5); 
  fanServo.attach(6);

  Wire.begin(); 
}
void loop() {
  receiveWireless();
  if (txData.power == powerON){
    setMode();
  }
  if (oldData.power != txData.power) {
    changePower();
  }
  if (oldData.fan != txData.fan) {
    changeFan(); 
  }
  oldData = txData;
  i2cTransmit();  
}
void i2cTransmit() {
  Serial.println("Sending"); 
  Wire.beginTransmission(8); 
  Wire.write(txData.currentTemp); 
  Wire.endTransmission(); 
}
void changeFan() {
  switch (txData.fan) {
    case 1:
      setFan(fanLOW); 
      break;
    case 2:
      setFan(fanMED);
      break;
    case 3: 
      setFan(fanHI); 
      break;
  }
}
void changePower() {
  if (txData.power == 0) {
    setPower(powerOFF);
    Serial.println("Power off"); 
  }
  else {
    setPower(powerON);
    Serial.println("Power on"); 
  }
}
void setMode() {
  switch(txData.mode) {
    case 1:
      heatMode(); 
      break;
    case 2:
      coolMode();
      break;
    case 3:
      autoMode(); 
      break;
    case 4:
      weatherMode();
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
    */
  }
  delay(500); 
}

