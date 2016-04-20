#include <SPI.h>
#include "RF24.h"

RF24 radio(9,10); 
byte addresses[][6] = {"1Node","2Node"};

// Structures, Holy SHIT
struct dataStruct {
  int currentTemp; 
  int setTemp; 
  int fan; 
  int power; 
  int mode; 
} txData;

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
}

void loop() {
  if (radio.available())
  {
    Serial.println("Radio available"); 
    while (radio.available()) 
    {
      radio.read( &txData, sizeof(txData)); 
    }
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
  }
  delay(500); 
}
