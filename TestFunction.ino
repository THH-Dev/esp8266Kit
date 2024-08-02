/* library install

- Add File >> preference: https://arduino.esp8266.com/stable/package_esp8266com_index.json
- Driver:     https://www.wemos.cc/en/latest/ch340_driver.html
- Schematic:  https://www.wemos.cc/en/latest/_static/files/sch_d1_mini_pro_v2.0.0.pdf
              https://www.wemos.cc/en/latest/_static/files/sch_d1_mini_v4.0.0.pdf
- Pinout:     https://s3.amazonaws.com/randomnerdtutorials/jhdfsASDFJEWJjsdfajdsafJDAFSJafd/ESP8266_Pinout_Diagrams.pdf
-Tutorial:    https://www.wemos.cc/en/latest/tutorials/index.html


Library: (Sketch >> Include Library >> Manager Libraries ... )
-ESP8266 Weather Station (link: https://github.com/ThingPulse/esp8266-oled-ssd1306)


Hardware:
- LCD: GME12864, SSD1306 0.96 Inch (https://vietnamese.alibaba.com/product-detail/Most-Popular-GME12864-White-4pins-I2C-1600270997819.html)


References:
- https://www.instructables.com/3D-Printer-Monitor-Wemos-D1-Mini-ESP8266/
- https://randomnerdtutorials.com/esp8266-0-96-inch-oled-display-with-arduino-ide/
- MQtt: https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-bme680-arduino/
- Interrupt UART: https://github.com/clarkwise/ESP8266_UART_TOUT_INT_clarkwise_handler?tab=readme-ov-file

*/
#include "my_lib.h"

#include "myOLED.h"
#include "myRFID.h"
#include "myMqtt.h"

MyOLED* myOLED = MyOLED::GetInstance();
MyRFID* myRFID = MyRFID::GetInstance();
MyMQTT* myMQTT = MyMQTT::GetInstance();



/*------------------------------------------------------
                      INT - Interrupt
--------------------------------------------------------
*/
volatile bool buttonFlag = false;

ICACHE_RAM_ATTR void sensorTrigger() {
  buttonFlag = !buttonFlag;
  //toogleLed();
}

void setup() {

  //Interrupt
  pinMode(13, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(13), sensorTrigger, RISING);

  //Display
  myOLED->OLED_Init();
  myOLED->OLED_Demo(1);
  myOLED->OLED_ShowString(8, 2, "RFID Data", 8);
  //Rfid
  myRFID->init();
  //Init serial
  //Serial.begin(38400);

  //mqtt
  myMQTT->mqttInit();
}

bool bState = false;
void toogleLed() {
  digitalWrite(2, bState);  // bật đèn led
  bState = !bState;
}
int i = 0;
void loop() {

  //Trigger from sensor
  // if(buttonFlag == true)
  // {
  //   buttonFlag = false;
  //   toogleLed();
  //   //myRFID->trigger();
  //   Serial.println("Interrupt!");

  // }



  //toogleLed();
  //Serial.println(String("Interrupt" + buttonFlag?"true":"false"));

  
  myRFID->trigger();
  int queueSize = myRFID->queueData.count();
  if (queueSize > 0) {

    String epcData = myRFID->queueData.pop();
    if (!epcData.isEmpty()) {
      String epcData2 = String("@" + String(queueSize) + "$" + epcData);
      Serial.println(epcData2);
      myMQTT->mqttPublic(epcData.c_str());
      
      myOLED->OLED_Clear();
      myOLED->OLED_ShowString(8, 2, "RFID Data", 8);
      myOLED->OLED_ShowString(8, 8, epcData.c_str(), 8);

    } else {
      Serial.println("EPC is empty");
      myOLED->OLED_Clear();
      myOLED->OLED_ShowString(8, 2, "RFID Data", 8);
      myOLED->OLED_ShowString(8, 8, "EPC Empty", 8);
    }


  } else {
    //Serial.println("null");
  }/**/

  //myMQTT->mqttPublic(String("Hello " + i).c_str());
  //myMQTT->mqttPublic(String("Hello------------"));
  // i++;
  // if(i>=10000)
  //   i = 0;
  delay(3000);
}
