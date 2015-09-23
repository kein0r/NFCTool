#include <TwoWirePlus.h>
#include "PN532.h"

#define PN532_BUFFERSIZE (uint8_t)40
#define PN532_ACKBUFFERSIZE (uint8_t)6

/* The amount of data that can be exchanged using this frame structure is limited 
 * to 255  bytes (including TFI). */
PN532_Data_t data[PN532_BUFFERSIZE];
PN532_Data_t ackData[PN532_ACKBUFFERSIZE];

PN532 nfc(0x24);

void setup()
{
  uint8_t numBytesReceived = 0;
  Serial.begin (115200);
  Serial.println("Go");
  nfc.sendCommand(PN532_CommandCode_GetFirmwareVersion, data, 0);
  Serial.println("Command send");
  numBytesReceived = nfc.receiveResponse(data, 5);
  Serial.print("Response received ");Serial.print(numBytesReceived);Serial.println(" Bytes"); 
}  // end of setup

void loop()
{
  
}
