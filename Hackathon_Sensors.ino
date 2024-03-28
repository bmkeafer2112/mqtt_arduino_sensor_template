/* CTRL-ALT-COMPETE
 *  This program can handle up to four sensors.
 *  The data is transmitted via an MQTT topic, and the internal counting
 *  variable can be set over an MQTT topic.  See the BrokerConfig.h tab
 *  for wifi, broker, and topic settings.
 *  
 *  
 *  MQTT Topics (See broker settings):
 *  TOPIC_SENSOR1_VALUE   The numaric count of sensor triggers.
 *  TOPIC_SET_VALUE   The count requested by a seperate client.  Currently
 *                    only a reset function.
 *  
 *  Original_Version: 0.0.4
 *  Original Author: R. D. MacBride
 *  Modified: C. Sweat 2/14/24
 *  
 *  For questions, call (757)503-3257
 */

/* External libraries and drivers
 *  -Arduino SAMD Boards
 *  -WifiNINA 1.8.13
 *  -PubSubClient 2.8.0
 */

/* Hardware
    Pre-configured setup. 
    Value1 is setup for Digital pin #3 by using mode #1
    Value2 is setup for Analog pin #0 by using mode #2
    Value3 is setup for Special Case pin #12(only. Unless specified in declaration) by using mode #3
    Value4 is setup for Digital pin #5 by using mode #1
 */

//Definitions
#define SENSE1 3             //Sensor 3 pin
#define SENSE2 0             //Sensor 0 pin                               
#define SENSE3 12             //Sensor 12 pin
#define SENSE4 12              //Sensor 12 pin
#define MODE1 1             //Sensor 1 pinMode
#define MODE2 2             //Sensor 2 pinMode
#define MODE3 3             //Sensor 3 pinMode
#define MODE4 4             //Sensor 4 pinMode
#define DEBOUNCE_DELAY1 10    //Debouncing delay
#define DEBOUNCE_DELAY2 10    //Debouncing delay
#define DEBOUNCE_DELAY3 10    //Debouncing delay
#define DEBOUNCE_DELAY4 10    //Debouncing delay
#define REPORT_DELAY 5000     //Serial monitor report delay
#define PUBLISH_DELAY 1000    //MQTT publish delay


//Libraries
#include <SPI.h>
#include <WiFiNINA.h>           //Wifi Library
#include <PubSubClient.h>       //MQTT Library
#include "BrokerConfig.h"       //VLAN and MQTT broker connection details
#include "Debounce.h"           //Debouncer Utility


//Debounced sensor inputs
Debounce Sensor1(SENSE1, DEBOUNCE_DELAY1, MODE1);   //Debounced input 1 class
Debounce Sensor2(SENSE2, DEBOUNCE_DELAY2, MODE2);   //Debounced input 2 class
Debounce Sensor3(SENSE3, DEBOUNCE_DELAY3, MODE3);   //Debounced input 3 class
Debounce Sensor4(SENSE4, DEBOUNCE_DELAY4, MODE4);   //Debounced input 4 class


//Event timers
unsigned long publishTimer = 0;   //Last publish to the MQTT broker
unsigned long reportTimer = 0;    //Last report to the serial monitor


//Box counting variables
int value1 = 0;                    //Box count since last reset.
int value2 = 0;                    //Box count since last reset
int value3 = 0;                    //Box count since last reset
int value4 = 0;                    //Box count since last reset
int pubValue1 = 0;                 //Last published box count
int pubValue2 = 0;                 //Last published box count
int pubValue3 = 0;                 //Last published box count
int pubValue4 = 0;                 //Last published box count

//MQTT Callback Definition
void mqttCallback(char* topic, byte* payload, unsigned int length);


//WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP, BROKER_PORT, mqttCallback, wifiClient);


void setup() {
  //Start serial communication to monitor
  Serial.begin(9600);

  //Wait for connection, but timeout after half a second and continue
  //Serial communication is good for debugging, but this device normally
  //runs without it.  The delay allows for cleaner debug monitoring.
  while (!Serial && millis() < 500);
  
  Serial.println("MQTT Sensors Started!");

  //Establish an initial connection to the broker.
  //The same function runs later if the connection gets interrupted.
  connectToBroker();
}


void loop() {
  //Maintain a connection to the broker.
  mqttClient.loop();


  //Publish the value on timer
  if(millis() - publishTimer > PUBLISH_DELAY){
    publishTimer = millis();
    pubValue1 = value1;
    pubValue2 = value2;
    pubValue3 = value3;
    pubValue4 = value4;

    char buffer[33];
      if(mqttClient.publish(TOPIC_SENSOR_VALUE1, itoa(pubValue1, buffer, 10))){
        Serial.print(TOPIC_SENSOR_VALUE1);
        Serial.print(": ");
        Serial.println(itoa(pubValue1, buffer, 10));
      }else{
        Serial.println("Warning: Publish Failed!");
        connectToBroker();
      }
      if(mqttClient.publish(TOPIC_SENSOR_VALUE2, itoa(pubValue2, buffer, 10))){
        Serial.print(TOPIC_SENSOR_VALUE2);
        Serial.print(": ");
        Serial.println(itoa(pubValue2, buffer, 10));
      }else{
        Serial.println("Warning: Publish Failed!");
        connectToBroker();
      }
      if(mqttClient.publish(TOPIC_SENSOR_VALUE3, itoa(pubValue3, buffer, 10))){
        Serial.print(TOPIC_SENSOR_VALUE3);
        Serial.print(": ");
        Serial.println(itoa(pubValue3, buffer, 10));
      }else{
        Serial.println("Warning: Publish Failed!");
        connectToBroker();
      }
      if(mqttClient.publish(TOPIC_SENSOR_VALUE4, itoa(pubValue4, buffer, 10))){
        Serial.print(TOPIC_SENSOR_VALUE4);
        Serial.print(": ");
        Serial.println(itoa(pubValue4, buffer, 10));
      }else{
        Serial.println("Warning: Publish Failed!");
        connectToBroker();
      }
  }
  
  //Perform an input update and check for a state change from low to high. 
  // This is where you will update the values to be sent to the MQTT broker.


  //for use with digital
  if(Sensor1.heartbeat() && Sensor1.state() == HIGH){value1 = 1;}else{value1 = 0;};
  //for use with analog
  if(Sensor2.heartbeat() == HIGH){value2 = Sensor2.state();}else{};
  if(Sensor3.heartbeat() == HIGH){value3 = Sensor3.state();}else{};
  if(Sensor4.heartbeat() == HIGH){value4 = Sensor4.state();}else{};
}

//Process incomming contents to a subscribed topic
//There is only one, otherwise we would need to figure out the topic.
void mqttCallback(char* topic, byte* payload, unsigned int length){
    value1 -= pubValue1;
    value2 -= pubValue2;
    value3 -= pubValue3;
    value4 -= pubValue4;
    Serial.println("Count Reset!");
  //}
}

//Connect to WiFi and the broker.  Set up MQTT settings.
void connectToBroker(){
  //Disconnect from the broker if connected.
  //After an initial connection, PubSubClient always thinks its connected, and won't attemt
  //a reconnect.  If the program is here, the connection is no good.
  mqttClient.disconnect();

  //Attempt to connect to the network if not connected.
  if(WiFi.status() != WL_CONNECTED){
    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(VLAN_SSID);

    //Set a static IP - There is a known issue assigning a static IPs on Nina.
    //Currently using DHCP reservation


    //Attempt to connect to WiFi 5 times.
    int i = 0;
    while (!WiFi.begin(VLAN_SSID, VLAN_PASS)) {
      Serial.print(".");
      delay(1000);
      i++;
      if(i >= 5) break;
    }

    //Provide WiFi post-setup information
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("Connected to Network!");
      Serial.println();

      //Check Firmware of the wireless module.  
      String fv = WiFi.firmwareVersion();
      Serial.print("Firmware: ");
      Serial.println(fv);
      if (fv != "1.0.0") {
        Serial.println("Please upgrade the firmware");
      }
    
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
    
      Serial.print("Subnet Mask: ");
      Serial.println(WiFi.subnetMask());

      Serial.print("Gateway IP: ");
      Serial.println(WiFi.gatewayIP());

      printCurrentNet();
      printWiFiData();
    }else{
      Serial.println("Connection to Network Failed!");
    }
    Serial.println();
  }


  //Connect to the broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.print(BROKER_IP);
  Serial.print(":");
  Serial.println(BROKER_PORT);

  //Attempt to connect to the broker 5 times.
  int i = 0;
  while (!mqttClient.connect(BROKER_DEVICE, BROKER_USER, BROKER_PASS)) {
    Serial.print(".");
    delay(1000);
    i++;
    if(i > 5) break;
  }

  //Provide broker post-setup information
  if(mqttClient.connected()){
    
    //Subscribe to set topic
    mqttClient.subscribe(TOPIC_SET_VALUE);
  
    Serial.println("Connected to Broker!");
    Serial.println();
  }else{
    Serial.println("Connection to Broker Failed!");
    Serial.println();
  }
}

//Convert MAC array to string
//Code from getmicros.net
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printWiFiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);
  Serial.println();
}
