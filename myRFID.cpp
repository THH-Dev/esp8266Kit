#include "MyRFID.h"

MyRFID* MyRFID::pinstance_{ nullptr };


MyRFID* MyRFID::GetInstance() {
  if (pinstance_ == nullptr) {
    pinstance_ = new MyRFID();
  }
  return pinstance_;
}

void MyRFID::trigger() {
  Serial.print("\nQ\r");
}

void MyRFID::init() {
  //Init serial
  Serial.begin(38400);

  // Initialize UART interrupt
  install_uart_interrupt();
}

void MyRFID::processData(char incomingByte) {
  switch (incomingByte) {
    case '\n':  // <LF> character
      if (!receivingData) {
        receivingData = true;
        bufferIndex = 0;
        memset(buffer, 0, bufferSize);
      } else {
        receivingData = false;
        buffer[bufferIndex] = '\0';  // Null-terminate the buffer
        String rfidData(buffer);
        if (!rfidData.equals("X") && !rfidData.equals("Q")) {

          //Push to queue
          int data_length = rfidData.length();
          if (data_length >= 29) {
            String dataCopy = rfidData.substring(5, 29);  // Create a copy to store in the queue
            //Serial.println(String("#" + dataCopy));       // Process the received data (print to Serial Monitor)
            queueData.push(dataCopy);
          }
          else
          {
            String dataCopy = rfidData;  // Create a copy to store in the queue
            Serial.println(String("*" + dataCopy));       // Process the received data (print to Serial Monitor)
            queueData.push(dataCopy);
          }




        } else if(rfidData.equals("X")) {
          // TODO: Define care X case
          queueData.push("Error RFID!");
          //Serial.println("Error RFID!");
        }
        else if(rfidData.equals("Q")) {
          // TODO: Define care X case
          queueData.push("Can't read EPC, no tag found!");
          //Serial.println("Can't read EPC, no tag found!");
        }
        bufferIndex = 0;  // Reset the buffer index for next packet
      }
      break;

    case '\r':  // <CR> character
      // Ignore, waiting for the subsequent <LF>
      break;

    default:
      if (receivingData && bufferIndex < bufferSize - 1) {
        buffer[bufferIndex++] = incomingByte;  // Store the byte in the buffer
      }
      break;
  }
}

void MyRFID::install_uart_interrupt() {
  ETS_UART_INTR_DISABLE();                            //Disable UART Interrupt
  ETS_UART_INTR_ATTACH(uart0_rx_intr_handler, NULL);  //attach handler function to uart0_rx_intr_handler

  WRITE_PERI_REG(UART_CONF1(0), UART_RX_TOUT_EN | ((0x2 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S));  //Enable UART RX Timeour function and set the timeout period as the time transmitting 2 bits

  WRITE_PERI_REG(UART_INT_CLR(0), 0xffff);                         //Clear UART Interrupt flags
  SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_TOUT_INT_ENA);    //Enable UART RX Timeout interrupt
  CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);  //Disable UART RX Full interrupt
  ETS_UART_INTR_ENABLE();
}

void IRAM_ATTR MyRFID::uart0_rx_intr_handler(void* para) {
  MyRFID* p = MyRFID::GetInstance();  // Get singleton instance
  uint8_t uart_no = UART0;
  uint8_t fifo_len = 0;
  uint32_t uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no));  //get uart intr status
  //Serial.print(String(uart_intr_status));

  if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST)) {  //if it is caused by a time_out interrupt
    //Serial.println("OK");
    fifo_len = (READ_PERI_REG(UART_STATUS(uart_no)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;  //read rx fifo length
    //char incomingByte = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
    //Serial.print(incomingByte);
    //process data
    //p->processData(incomingByte);
    while (fifo_len > 0) {
      char incomingByte = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
      p->processData(incomingByte);
      fifo_len--;
    }

    WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_TOUT_INT_CLR);  //clear full interrupt state
  }
  uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no));  //update interrupt status
}
