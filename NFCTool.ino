#include <Wire.h>
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
  Serial.begin (115200);
  Wire.begin();
}  // end of setup

void loop()
{
  nfc.sendCommand(PN532_CommandCode_GetFirmwareVersion, data, 0);
  nfc.receiveAckFrame(ackData);
  delay (1000);
}
