#include "myMqtt.h"

MyMQTT *MyMQTT::pinstance_{nullptr};

MyMQTT *MyMQTT::GetInstance()
{
  if (pinstance_ == nullptr)
  {
    pinstance_ = new MyMQTT();
  }
  return pinstance_;
}

void MyMQTT::mqttPublic(String data)
{
  if(!bIsConnected)
  {
    Serial.println("Not yet connect to mqtt, return!");
    return;
  }
  //public
  mqttClient.publish(MQTT_PUB_TRANSMIT_DATA, 2, true, data.c_str());

}
void MyMQTT::mqttInit()
{

  Serial.println("-----------------");
  Serial.println("-----------------");
  Serial.println(MQTT_HOST);
  Serial.println(WIFI_SSID);
  Serial.println(WIFI_PASSWORD);

  wifiConnectHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP &event)
                          {
    Serial.print("Station mode got IP: ");
    Serial.println(event.ip.toString());
    // Process event information


    Serial.println("Connected to Wi-Fi, begin to connect to mqtt");
    this->connectToMqtt(); });

  wifiDisconnectHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected &event)
                                 {
    Serial.println("Station mode disconnected.");
    // Process event information
    Serial.println("Disconnected from Wi-Fi.");
    this->mqttReconnectTimer.detach();  // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    this->wifiReconnectTimer.once(2, [this]() {
            this->connectToWifi();
        }); });


  mqttClient.onConnect([this](bool sessionPresent)
                       { this->onMqttConnect(sessionPresent); });

  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason)
                          { this->onMqttDisconnect(reason); });

  // mqttClient.onSubscribe([this](uint16_t packetId, uint8_t qos)
  //                        { this->onMqttSubscribe(packetId, qos); });

  mqttClient.onUnsubscribe([this](uint16_t packetId)
                           { this->onMqttUnsubscribe(packetId); });

  mqttClient.onMessage([this](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
                       { this->onMqttMessage(topic, payload, properties, len, index, total); });

  // mqttClient.onPublish([this](uint16_t packetId)
  //                      { this->onMqttPublish(packetId); });

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}
void MyMQTT::connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

}

void MyMQTT::connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void MyMQTT::onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void MyMQTT::onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, [this]()
                          { this->connectToWifi(); });
}

void MyMQTT::onMqttConnect(bool sessionPresent)
{
  bIsConnected = true;

  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe(MQTT_PUB_RECEIVED_CMD, 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);

}

void MyMQTT::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  bIsConnected = false;

  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, [this]()
                            { this->connectToMqtt(); });
  }
}

void MyMQTT::onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);

  Serial.print("  qos: ");
  Serial.println(qos);
}

void MyMQTT::onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void MyMQTT::onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);

  Serial.print("  payload: ");
  Serial.println(payload);
  
  String newString(payload);
  Serial.println(newString.substring(0,len));


  Serial.print("  qos: ");
  Serial.println(properties.qos);

  Serial.print("  dup: ");
  Serial.println(properties.dup);

  Serial.print("  retain: ");
  Serial.println(properties.retain);

  Serial.print("  len: ");
  Serial.println(len);

  Serial.print("  index: ");
  Serial.println(index);

  Serial.print("  total: ");
  Serial.println(total);
}

void MyMQTT::onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}