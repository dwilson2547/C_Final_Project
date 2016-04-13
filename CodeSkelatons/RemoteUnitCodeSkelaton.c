#include <LiquidCrystal.h>
#include <radiofile.h>
int setP; 
int currentTemp; 
LiquidCrystal lcd(12,11,5,4,3,2); 
// temperature sensor on pin 0
// potentiometer on pin 1
// lcd on pins 12,11,5,4,3,2

void setup() {
	// initialize 2.4 GHZ module
	// initialize screen
	lcd.begin(16,2);
}

void loop(){
	checkTemp(); 
	writeScreen(); 
	sendData(); 
}

void sendData() {
	// transmit data to receiver unit
}
void checkTemp() {
	// check real temperature
	// change setpoint to temperature 
	currentTemp = analogRead(0) * 5 / 1023;
	// check setpoint from potentiometer
	setP = analogRead(1) * 5 / 1023; 
	
}
void writeScreen() {
	// write current temperature and setpoint to screen
	lcd.setCursor(0,1);
	lcd.print("Setp: ");
	lcd.print(setP); 
	lcd.setCursor(0,2); 
	lcd.print("Current: ");
	lcd.print(currentTemp); 
}