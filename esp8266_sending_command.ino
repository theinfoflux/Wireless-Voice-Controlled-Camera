
//ip address 10.185.230.216
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DFRobot_DF2301Q.h"
///////////////////////////////////These libraries are for interfacing DHT sensor

//I2C communication
DFRobot_DF2301Q_I2C asr;
#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router


const char* ssid = "Xiaomi13T"; //--> Your wifi name or SSID.
const char* password = "12345678"; //--> Your wifi password.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ON_Board_LED,OUTPUT);

 // Init the sensor
  while (!(asr.begin())) {
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");

  /**
   * @brief Set voice volume
   * @param voc - Volume value(1~7)
   */
  asr.setVolume(4);

  /**
     @brief Set mute mode
     @param mode - Mute mode; set value 1: mute, 0: unmute
  */
  asr.setMuteMode(0);

  /**
     @brief Set wake-up duration
     @param wakeTime - Wake-up duration (0-255)
  */
  asr.setWakeTime(20);

  /**
     @brief Get wake-up duration
     @return The currently-set wake-up period
  */
  uint8_t wakeTime = 0;
  wakeTime = asr.getWakeTime();
  Serial.print("wakeTime = ");
  Serial.println(wakeTime);

asr.playByCMDID(1);   // Wake-up command

  /**
     @brief Play the corresponding reply audio according to the ID
     @param CMDID - command word ID
  */
  asr.playByCMDID(23);  // Command word ID

  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------
}

void loop() {
   uint8_t CMDID = asr.getCMDID();
  switch (CMDID) {
    case 50: 
     Serial.println("Take photos and save them");//If the command is “Turn on the light”
     digitalWrite(ON_Board_LED,HIGH);  
     sendCaptureCommand();
     digitalWrite(ON_Board_LED,LOW);  
    break;
    default:
      if (CMDID != 0) {
        Serial.print("CMDID = ");  //Printing command ID
        Serial.println(CMDID);
      }
  }
  delay(300); 
}

void sendCaptureCommand() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;  // create WiFiClient object
    HTTPClient http;
    String url = "http://10.185.230.216/capture";  // ✅ ESP32-CAM IP here

    http.begin(client, url);   // ✅ new API syntax
    int httpCode = http.GET(); // send GET request

    if (httpCode > 0) {
      Serial.printf("Response code: %d\n", httpCode);
    } else {
      Serial.printf("Request failed: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}
