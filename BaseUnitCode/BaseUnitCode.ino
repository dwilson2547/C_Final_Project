int setP; 
int currentTemp; 
int heat; 
#include <Servo.h>
//#include <radiofile.h>

int pin = 1; 
int pin1 = 2;

int cool; 
int srvOnOff; 
int powerOn; 
int powerOff; 
Servo srvPower; 
Servo srvTemp;

void setup() {
  // initialize 2.4 GHZ module
  // initialize servos
  srvPower.attach(pin); 
  srvTemp.attach(pin1); 
}

void loop(){
  receiveData(); 
  checkTemp(); 
}

void receiveData() {
  // get data from 2.4GHZ module
  // change setP
  // change currentTemp
}
void checkTemp() {
  if (currentTemp < setP - 3) {
    // turn power on
    power(1); 
    // set temperature position
    changeTemp(heat); 
  }
  else if (currentTemp > setP + 3) {
    // turn on power
    power(1); 
    // set temperature point
    changeTemp(cool); 
  }
  else {
    // turn off
    power(0); 
  }
}
void changeTemp(int pos) {
  // temp servo position write pos
  srvTemp.write(pos); 
}
void power(int pos) {
  if (pos == 0) {
    // make servo go to power off position
    srvPower.write(powerOff);  
  }
  else {
    // make servo go to power on position
    srvPower.write(powerOn); 
  }
}
