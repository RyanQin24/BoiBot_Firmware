#include <SoftwareSerial.h>

//Sends the potenchiometer (left and right) position to the ESP8285 Wifi Module on the remote control

//Hardware pin config variables
const int RX = 2;
const int TX = 3;
const int userAdd = 11;

//variables for potenchiometer position
int LeftPowStick = 0;
int RightSteerStick = 0;
String motorout;

// Initializing software Serial pin config
SoftwareSerial mySerial(RX, TX); 


void setup() {

  //Serial initialization
  mySerial.begin(74880);
  Serial.begin(74880);
}

void loop() {
    //Encoding integer motor power data and sending Serial Data Packets
    LeftPowStick = analogRead(A0);
    RightSteerStick = analogRead(A1);
    motorout = String(LeftPowStick) + "," + String(RightSteerStick) + ".";
    mySerial.println(motorout);
    Serial.println(motorout);
    delay(20);
}