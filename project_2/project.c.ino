/* Dan Milliken */
/* CNIT 280 Project */

#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#define csv_RecordBuffer 20
#define csv_FieldTempBuffer 3
#define csv_field 2


// Enter a MAC address and IP address for your controller below.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01
};

IPAddress ip(192, 168, 1, 177);

// Init server port
EthernetServer server(80);

// Globals
// bool sd_exists;
char csv_Record[csv_FieldTempBuffer];

void setup() {
  // Wire connection
  Wire.begin();
  Wire.onReceive(receiveCSV);
  
  // Open serial comm and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Prep SD card
  pinMode(53, OUTPUT);
  if(!SD.begin(4)) {
    // sd_exists = false;
    Serial.println("SD Card INIT failed");
    
  }
  else {
    // sd_exists = true;
    Serial.println("SD Card INIT success");
  }
  
  // start webserver
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server Running on ");
  Serial.println(Ethernet.localIP());
}


void loop() { 
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      Serial.println("Client Connected");
      if (client.available()) {
        Serial.println("Client Available");
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        
        if (c == '\n' && currentLineIsBlank) {
          // send http response header
          Serial.print("Sending HTTP Response Header to ");
          Serial.println();
          client.println("HTTP/1.1 200 OK");
          client.println("Server: Arduino/1.0");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();

          //Output HTML Top Content to client
          html_file_output(&client, "top.htm");

          //Output HTML Temp data to client
          client.println("<p>Recent Temperatures</p>");
          client.println("<table>");
          client.println("<tr><th>Room Temp</th><th>Control Temp</th></tr>");
          read_csv_file(&client, "temp.csv", csv_field, 3);
          client.println("</table>");
          //Output HTML Bottom Content to client
          html_file_output(&client, "bottom.htm");
        }
        if (c == '\n') {
          // Starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // Character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected");
  }
}


// Interupt Function for data on I2C wire
void receiveCSV(int howMany) {
  Serial.println("Wire Interrupt received");
  int i =0;
  char csv[csv_RecordBuffer];
  /* Read while wire signal and do not exceed string buffer */
  while (1 < Wire.available() && i < (csv_RecordBuffer-2)) { 
    csv[i] = Wire.read();
    i++;       
    Serial.print(csv[i]);           
  }
  csv[i] = '\0';
  save_csv_file(csv,"temp.csv");
  /* Call File Save Opertion 
  if(sd_exists) {
    Serial.println("SD Card Exists. Saving Wire Transfer.");
    save_csv_file(csv,"temp.csv");
  }
  */
}

/*
// Send HTML page test
void html_test_output(EthernetClient* client) {
  Serial.println("HTML test begin");
  client->println("<!DOCTYPE HTML>");
  client->println("<html>");
  client->println("<H1>Hillenbrand Room Temperature Status</H1>");
  client->println("<p>Hello World!</p>");
  client->println("</html>");
  Serial.println("HTML test end");
}
*/

// Send HTML to client from source file
void html_file_output(EthernetClient* client, String filename) {
  File filedata = SD.open(filename);
  char c;
  int n;
  if(filedata.available()) {
      Serial.print("File: ");
      Serial.print(filename);
      Serial.println(" opened for read");
      while(filedata.available()) {
        c = filedata.read();
        if(c=='\n') {
          n++;
        }
        Serial.print(c);
        client->print(c);
      }
      filedata.close();
      Serial.print("File: ");
      Serial.print(filename);
      Serial.println(" closed");
      Serial.print(n);
      Serial.print(" lines read from file");
  } 
  else {
    Serial.print("File: ");
    Serial.print(filename);
    Serial.println(" Read Failure");
  }
}


// Read CSV file records for client
void read_csv_file(EthernetClient* client, String filename, int fieldCount, int recordCount) {
  File filedata = SD.open(filename);
  int n;
  int i=0;
  int j;
  if(filedata.available()) {
      Serial.println("File: opened for reading");
      while(filedata.available() && i < recordCount) {
        j=0;
        client->print("<tr>");
        while(filedata.available() && j< fieldCount) {
          n = csv_RecordField(&filedata, ",");
          client->print("<td>");
          if (n > 0) {
            Serial.print("Field length ");
            Serial.print(n);
            Serial.print("read as ");
            Serial.println(csv_Record);
            client->print(csv_Record);
          }
          client->print("</td>");
          j++;
        }
        client->print("</tr>");
        i++;
      }
      filedata.close();
      /* 
      Serial.print("File: ");
      Serial.print(filename);
      Serial.println(" Closed for reading");
      Serial.print(i);
      Serial.println(" records read from file");
      Serial.print(j);
      Serial.println(" fields per record");     
      */ 
  } 
  else {
    Serial.print("File: ");
    Serial.print(filename);
    Serial.println(" Read Failure");
  }
}

/**** Read CSV file records for program manipulation
void read_csv_file(String filename) {
  File filedata = SD.open(filename);
  int n;
  int i=0;
  if(filedata.available()) {
      Serial.println("File: opened for reading");
      while(filedata.available()) {
        n = csv_RecordField(&filedata, ",");
        if (n > 0) {
          Serial.print("Field length ");
          Serial.print(n);
          Serial.print("read as ");
          Serial.println(csv_Record);
        }
        i++;
      }
      filedata.close();
    Serial.print("File: ");
    Serial.print(filename);
    Serial.println(" Closed for reading");
    Serial.print(i);
    Serial.println(" fields read from file");
      
  } 
  else {
    Serial.print("File: ");
    Serial.print(filename);
    Serial.println(" Read Failure");
  }
}
********************/


// Parse fields from file by using delimiter
int csv_RecordField(File* filedata, const char* delimiter) {
  char c;
  int i = 0;
  // Read until end of buffer and leave 1 character for null terminator
  while (i < (csv_RecordBuffer - 2) && filedata->read(&c, 1) == 1) {
    // Break on new Record or delimiter
    if(c == '\n' || strchr(delimiter, c) ) {
      break;
    }
    csv_Record[i] = c;
    i++;
  }
  csv_Record[i] = '\0';
  return i;
}

// Save string record to CSV file
void save_csv_file(char* record, String filename) {
  File filedata = SD.open(filename, FILE_WRITE);
  int i=0;
  if(filedata.available()) {
    while(filedata.available() && (record[i] != '\0') && (i <= (csv_RecordBuffer-1)) ) {
      filedata.print(record[i]);
      i++;
    }
    filedata.close();
    Serial.print("Record length: ");
    Serial.print(i);
    Serial.print(" written to ");
    Serial.println(filename);
    Serial.print("Record Value: ");
    Serial.println(record);
  } 
  else {
    Serial.print("File: ");
    Serial.print(filename);
    Serial.println(" Write Failure");
  }
}

/*** Print byte value ip
void printIP() {
  for (int bcount= 0; bcount < 4; bcount++) {
    Serial.print(rip[bcount], DEC);
    if (bcount<3) Serial.print(".");
  }
  Serial.println();
}
***/
