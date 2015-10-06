#include <TwoWirePlus.h>
#include "PN532.h"

#define PN532_BUFFERSIZE (uint8_t)40
#define PN532_ACKBUFFERSIZE (uint8_t)6

/* The amount of data that can be exchanged using this frame structure is limited 
 * to 255  bytes (including TFI). */
PN532_Data_t data[PN532_BUFFERSIZE];
PN532_Data_t ackData[PN532_ACKBUFFERSIZE];
uint8_t numBytesReceived = 0;

PN532 nfc(0x24);

typedef enum {
  NFCTool_State_Init,
  NFCTool_State_InListPassiveTarget_TypeA,
  NFCTool_State_InReleaseTarget
} NFCTool_State_t;

void setup()
{
  Serial.begin (115200);
  Serial.println(" Start!");
  //nfc.sendCommand(PN532_CommandCode_GetFirmwareVersion, data, 0);
  //numBytesReceived = nfc.receiveResponse(data, 5);
  //data[0] = 0x01;
  //data[1] = 0x3; /* 0x02 AutoRFCA, 0x01 RF On */
  //nfc.sendCommand(PN532_CommandCode_RFConfiguration, data, 2);
  //numBytesReceived = nfc.receiveResponse(data, 1);
  
  /* According to Chapter 3.1.3.8 Initialization sequence PN532_CommandCode_SAMConfiguration 
   * with mode 0x01 (normal mode) or mode 0x02 (virtual card mode) must be send after power-up */
  data[0] = 0x01; /* 0x01 normal mode */
  data[1] = 0x00; /* Timeout, onyl used when mode equals to 0x02 */
  data[2] = 0x01; /* IRQ, default 0x01 */
  nfc.sendCommand(PN532_CommandCode_SAMConfiguration, data, 3);
  numBytesReceived = nfc.receiveResponse(data, 1);
  delay(1);
  /* Configure retry to activate a passive target to 0x80, default is infinit 0xff */
  data[0] = 0x05;
  data[1] = 0xff; /* MxRtyATR default 0xff infinite */
  data[2] = 0x01; /* MxRtyPSL default try twice */
  data[3] = 0x80; /* MxRtyPassiveActivation default 0xff infinite, set to 0x80 to not block in case no card detected */
  nfc.sendCommand(PN532_CommandCode_RFConfiguration, data, 4);
  numBytesReceived = nfc.receiveResponse(data, 1);
}  // end of setup

void loop()
{
  char command;
  static NFCTool_State_t NFCTool_State = NFCTool_State_Init;
  switch (NFCTool_State)
  {
    case NFCTool_State_Init:
      if (Serial.available())
      {
        command = Serial.read();
        switch (command)
        {
          case 'a':
            data[0] = 0x02;  /* max targets */
            data[1] = 0x00;  /* 0x00 106kbs (ISO/IEC14443 Type A), 0x01 212kbps (FeliCa polling), 0x02 424kpbs (FeliCa polling), 0x03 106kbps (ISO/IEC14443-3B Type B), 0x04 106kbps (Innovision Jewel) */
            nfc.sendCommand(PN532_CommandCode_InListPassiveTarget, data, 2);
            NFCTool_State = NFCTool_State_InListPassiveTarget_TypeA;
            delay(60);
          break;
          case 'r':
            data[0] = 0x00;  /* all targets */
            nfc.sendCommand(PN532_CommandCode_InRelease, data, 1);
            NFCTool_State = NFCTool_State_InReleaseTarget;
          break;
        }
      }
    break;
    case NFCTool_State_InListPassiveTarget_TypeA:
      /* Scan was started, see if we received something */
      numBytesReceived = nfc.receiveResponse(data, 20);
      if (numBytesReceived)
      {
        Serial.print("Command: 0x"); Serial.println(data[0], HEX);
        Serial.print("Targets: "); Serial.println(data[1]);
        for (int i=2; i<numBytesReceived; i++)
        {
          Serial.print(" 0x");Serial.print(data[i], HEX);
        }
        Serial.println();
        /* Something was received, switch back to init state */
        NFCTool_State = NFCTool_State_Init;
      }
    break;
    case NFCTool_State_InReleaseTarget:
      numBytesReceived = nfc.receiveResponse(data, 2);
      if (numBytesReceived)
      {
        Serial.print("Command: 0x"); Serial.println(data[0], HEX);
        Serial.print("Status: 0x"); Serial.println(data[1], HEX);
        /* Something was received, switch back to init state */
        NFCTool_State = NFCTool_State_Init;
      }
    break;
    default:
      /* do nothing */
    break;
  }
  delay(10);  
}
