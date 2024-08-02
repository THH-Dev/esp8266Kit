/*
- https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
- https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-bme680-arduino/


 */
#ifndef MYMQTT_H
#define MYMQTT_H
#include "my_define.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>


class MyMQTT
{

private:
	static MyMQTT *pinstance_;
  bool bIsConnected;
protected:
    MyMQTT():bIsConnected(false){}
    ~MyMQTT() {}

public:
 	MyMQTT(MyMQTT &other) = delete;
    void operator=(const MyMQTT &) = delete;
 	static MyMQTT *GetInstance();

	void connectToWifi();
	void connectToMqtt();
	void onWifiConnect(const WiFiEventStationModeGotIP &event);
	void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
	void onMqttConnect(bool sessionPresent);
	void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	void onMqttSubscribe(uint16_t packetId, uint8_t qos);
	void onMqttUnsubscribe(uint16_t packetId);
	void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
	void onMqttPublish(uint16_t packetId);

	void mqttInit();
	void mqttPublic(String data);

	AsyncMqttClient mqttClient;

  WiFiEventHandler wifiConnectHandler;
  WiFiEventHandler wifiDisconnectHandler;

	Ticker mqttReconnectTimer;
	Ticker wifiReconnectTimer;

};
#endif