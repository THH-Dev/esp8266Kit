/*
-Add queue library
-Queue: https://github.com/SMFSW/Queue/blob/master/examples/SimpleQueueStatic/SimpleQueueStatic.ino

*/
#ifndef myRFID_H
#define myRFID_H

#include "my_lib.h"
#include "uart_register.h"
#include "QueueList.h"


class MyRFID {

private:
  static MyRFID *pinstance_;
  //Variable for UART interrupt
  int bufferSize;      // Define the buffer size 256
  char buffer[256];    // Create a buffer to store the incoming data
  int bufferIndex;     // Index to keep track of the buffer position
  bool receivingData;  // Flag to indicate if data reception is ongoing


protected:
  MyRFID()
    : bufferSize(256), bufferIndex(0), receivingData(false), rfidData("") {
    //Buffer
    memset(buffer, 0, sizeof(buffer));

    //Init serial
    Serial.begin(38400);
  }

  ~MyRFID() {}

public:
  MyRFID(MyRFID &other) = delete;
  void operator=(const MyRFID &) = delete;

  static MyRFID *GetInstance();

  String rfidData;
  QueueList <String> queueData;
  void trigger();
  void init();
  // void processData(char incomingByte);
  void processData(char incomingByte);
  void install_uart_interrupt();

  // Static interrupt handler
  static void IRAM_ATTR uart0_rx_intr_handler(void *para);

  //void uart0_rx_intr_handler(void* para);
  // Non-static member function for processing data
};
#endif