// #define WIFI_SSID "THH"
// #define WIFI_PASSWORD "302phuvien"
#ifndef MY_DEFINE_H
#define MY_DEFINE_H

#ifndef THH
  #define WIFI_SSID "THH"
  #define WIFI_PASSWORD "302phuvien"
  #define MQTT_HOST IPAddress(192, 168, 68, 43)
#else
  #define WIFI_SSID "Anh Thu"
  #define WIFI_PASSWORD "anh127790"
  #define MQTT_HOST IPAddress(192, 168, 1, 227)
#endif

#define MQTT_PORT 1883
#define MQTT_QoS 1

#define MQTT_PUB_TRANSMIT_DATA "esp/topic/data"
#define MQTT_PUB_RECEIVED_CMD "esp/topic/command"


// Temperature MQTT Topics
#define MQTT_PUB_TRANSMIT_DATA "esp/topic/data"
#define MQTT_PUB_RECEIVED_CMD "esp/topic/command"


#endif