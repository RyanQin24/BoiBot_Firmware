#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino_JSON.h>

//code takes arduino's serial output and prepares it to send to the Wifi module on the motor's motherboard

//recievers mac address
uint8_t broadcastAddress[] = {0xC4, 0xDD, 0x57, 0x02, 0x48, 0x90};

//variables 
 int Left = 0;
 int Right = 0;
 String LeftM = "475";
 String RightM = "475";

// Drivetrain data structure
typedef struct Drivetrain {
  int LeftPower;
  int RightPower;
} Drivetrain;

// Create a Drivetrain object called sense
Drivetrain sense;

//setting timeouts
unsigned long lastTime = 0;
unsigned long lastTimeTX = 0; 
unsigned long timerDelay = 20;  

// Callback(action to execute) when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void setup() {

  // Initalize Serial Monitor
  Serial.begin(74880);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer(the Wifi module attahced to the motor's motherboard)
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {

  //Checks is there any data in Serial buffer. Otherwise send the neutral position for potenchiometer.
  // decodes data in the form "'Left Potechiometer Position','Right Potenchiometer Position'."
  if(Serial.available()>0){  
    LeftM = Serial.readStringUntil(',');
    RightM = Serial.readStringUntil('.');
    lastTimeTX = millis();
 }else{
  LeftM = "475";
  RightM = "475";
 }

  //sets intervals to transmit data
  if ((millis() - lastTime) > timerDelay) {
    // Set motorPower values to send
    Left = LeftM.toInt();
    Right = RightM.toInt();
    Serial.println(Left);
    Serial.println(Right);
    sense.LeftPower = Left;
    sense.RightPower = Right;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &sense, sizeof(sense));

    lastTime = millis();
  }
}