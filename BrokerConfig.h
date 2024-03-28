/* Broker Congig
 *  This header file defines a series of configuration for connecting to WiFI,
 *  an MQTT broker, and setting MQTT topics
 */
//Libraries
#include <WiFiNINA.h>           //Wifi Library

//Set up the WiFi connection
#define VLAN_SSID "eng_week"                     //VLAN SSID
#define VLAN_PASS "Engin33r52024"                   //VLAN Password
byte mac[6];

//Set up the broker connection
#define BROKER_IP "192.168.105.22"
#define BROKER_PORT 1883
#define BROKER_USER NULL
#define BROKER_PASS NULL
#define BROKER_DEVICE "Arduino-7"

//Root topic path
#define TOPIC_ROOT      "CTRL-ALT-COMPETE/Arduino-Smart-Manufacturing"

//Specific topics under the root topic path
#define TOPIC_TIMER (TOPIC_ROOT "/t")             //(Publish) Regularly changing variable for connection timeouts
#define TOPIC_SENSOR_VALUE1 (TOPIC_ROOT "/Value1")  //(Publish) number of sensor triggers
#define TOPIC_SENSOR_VALUE2 (TOPIC_ROOT "/Value2")  //(Publish) number of sensor triggers
#define TOPIC_SENSOR_VALUE3 (TOPIC_ROOT "/Value3")  //(Publish) number of sensor triggers
#define TOPIC_SENSOR_VALUE4 (TOPIC_ROOT "/Value4")  //(Publish) number of sensor triggers
#define TOPIC_SET_VALUE (TOPIC_ROOT "/SetCount")  //(Subscribe) change the internal count variable
