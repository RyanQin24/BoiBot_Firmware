#include <ESP8266WiFi.h>
#include <espnow.h>

//ESP8285 reciever attached to motor's motherboard

//pin definintions for pwm and reverse transistor
int leftPWM = 12;
int rightPWM = 14;
int reversetransistor = 4;

//Low Pass Filter adjustment
int frequency = 1465;

// timeout,deadzone, and reverse logic safety variables
unsigned long lastTime = 0;    
unsigned long timeroutlim = 100;
int steerneutral = 130;
int pwrpothigh = 524; 
int pwrpotdeadlow = 495;
int steerpotdeadhigh = 524;
int steerpotdeadlow = 495;
int maxsteer = 255;
int minsteer = 0;
int minpwmoiw = 103;
int maxpwr = 130;

// Drivetrain data structure
typedef struct Drivetrain {
  int LeftPower;
  int RightPower;
} Drivetrain;

// Create a Drivetrain object called drive
Drivetrain drive;

//drivetrain conditional function
int drivetraincondition(int currentpowervalue){
  int retval;
 
//forward condition
 if (currentpowervalue > pwrpothigh){
    digitalWrite(reversetransistor, LOW);
    retval = map(currentpowervalue,pwrpothigh,1023,minpwmoiw,maxpwr);

//reverse condition
 }else if (currentpowervalue < pwrpotdeadlow){
    digitalWrite(reversetransistor, HIGH);
    delay(10);
    retval = 255;

//stop condition
 }else{
  digitalWrite(reversetransistor, LOW);
  return 0;
 }
 return retval;
}

//steering conditional function
int steercondition(int currentsteervalue){
  int retval;
 if (currentsteervalue < steerpotdeadlow){
    retval = map(currentsteervalue,steerpotdeadlow,0,steerneutral,maxsteer);
 } else if (currentsteervalue > steerpotdeadhigh){
    retval = map(currentsteervalue,steerpotdeadhigh,1023,steerneutral,minsteer);
 }else{
  return steerneutral;
 }
 return retval;
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&drive, incomingData, sizeof(drive));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("LeftPower: ");
  Serial.println(drive.LeftPower);
  Serial.print("RightPower: ");
  Serial.println(drive.RightPower);
  Serial.println();
  //function for drivatrain power
  drive.LeftPower = drivetraincondition(drive.LeftPower);
  drive.RightPower = steercondition(drive.RightPower);

  analogWrite(leftPWM, drive.LeftPower);
  analogWrite(rightPWM,drive.RightPower);
  Serial.print("RightPower out: ");
  Serial.println(drive.RightPower);

  lastTime = millis();
}


void setup() {

  //Initialize motor IO
  pinMode(leftPWM, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(reversetransistor, OUTPUT);
  analogWriteFreq(frequency);
  digitalWrite(leftPWM,LOW);
  digitalWrite(rightPWM,LOW);
  digitalWrite(reversetransistor, LOW);

  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Setting up the Wifi Module so it listend to remote's command
  // get recv packet info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  //Remote control timeout idle condition
  if (millis() - lastTime > timeroutlim){
    digitalWrite(leftPWM,LOW);
    digitalWrite(rightPWM,steerneutral);
    digitalWrite(reversetransistor, LOW);
  }
}

