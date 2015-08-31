/** @ingroup PN532
 * @{
 */

/*******************| Inclusions |*************************************/
#include "PN532.h"
#include <Wire.h>
#ifdef PN532_DEBUG
#include <Arduino.h>
#endif
/*******************| Macros |*****************************************/

/*******************| Type definitions |*******************************/

/*******************| Global variables |*******************************/

/*******************| Function Definition |****************************/

/**
 * Initializes PN532 module.
 * @pre Communication channel in use (I2C, SPI) already started
 */
PN532::PN532(uint8_t address) : PN532Address( address )
{
  
}


/**
 * Reads status byte from PN532.
 * In case data is ready no I2C stop is generated (Quote: "If the PN532 indicates that it is 
 * ready, the rest of the frame shall be read before sending an I2C STOP condition.")
 * @return status byte
*/
uint8_t PN532::statusReady()
{
  Wire.beginTransmission(PN532Address);
  /* Before initiating an exchange the host controller must write a byte indicating to the 
   * PN532 what the following operation is */
  Wire.write(PN532_FIRSTBYTE_STATUSREAD);
  /* Request two bytes without sending I2C_Stop */
  Wire.requestFrom(PN532Address, PN532_STATUSRAME_LENGTH, false);
  if (Wire.available())
  {
    return Wire.read();
  }
  return 0x00;
}

/**
 * Send a command to PN532 and check for ACK.
 * @param command Command to be sent
 * @param data Pointer to data to be sent
 * @param length Number of bytes to be send 
 * @return true if ACK frame was received, else false
 * @pre Address of PNE352 is defined
 */ 
bool PN532::sendCommand(PN532_CommandCode_t command, PN532_Data_t *data, uint8_t length)
{
  uint8_t checksum ;
  
  Wire.beginTransmission(PN532Address);
  /* Before initiating an exchange the host controller must write a byte indicating to the 
   * PN532 what the following operation is */
  Wire.write(PN532_FIRSTBYTE_DATEWRITE);

  /* Send fix header */  
  Wire.write(PN532_FRAME_PREAMBLE);
  Wire.write(PN532_FRAME_STARTCODE1);
  Wire.write(PN532_FRAME_STARTCODE2);

  /* length value sent to PN352 should include TFI and command byte therefore we add two bytes to LCS. */
  Wire.write((uint8_t)(length + 2));
  /* Send length checksum */
  Wire.write(~(uint8_t)((length + 2) + 1));
  
#ifdef PN532_DEBUG
  Serial.print(" 0x"); Serial.print(PN532_FRAME_PREAMBLE, HEX);
  Serial.print(" 0x"); Serial.print(PN532_FRAME_STARTCODE1, HEX);
  Serial.print(" 0x"); Serial.print(PN532_FRAME_STARTCODE2, HEX);
  Serial.print(" 0x"); Serial.print((uint8_t)(length + 2), HEX);
  Serial.print(" 0x"); Serial.print((uint8_t)(~(length + 2) + 1), HEX);
  Serial.print(" ("); Serial.print((~(length +2 )) + 1 + (length + 2), HEX);
#endif
  
  Wire.write(PN532_FRAME_TFI_HOSTTOCONTROLLER);
  checksum = PN532_FRAME_TFI_HOSTTOCONTROLLER;
  Wire.write(command);
  checksum += command;
  
#ifdef PN532_DEBUG
  Serial.print(") 0x"); Serial.print(PN532_FRAME_TFI_HOSTTOCONTROLLER, HEX);
  Serial.print(" 0x"); Serial.print(command, HEX);
#endif
  /* send data now. we increment first to remove  */
  for (int i=0; i< length; i++)
  {
    Wire.write(data[i]);
    checksum += data[i];
#ifdef PN532_DEBUG
    Serial.print(" 0x"); Serial.print(data[i], HEX);
#endif
  }
  /*  Data Checksum DCS byte that satisfies the relation: 
   *  Lower byte of [TFI + PD0 + PD1 + ... + PDn + DCS] = 0x00 */
  Wire.write(~checksum + 1);
  Wire.write(PN532_FRAME_POSTAMBLE);
#ifdef PN532_DEBUG
  Serial.print(" 0x"); Serial.print(uint8_t(~checksum + 1), HEX);
  Serial.print(" 0x"); Serial.print(PN532_FRAME_POSTAMBLE, HEX);
  Serial.println();
#endif
  Wire.endTransmission();
  
  while (! (statusReady() & PN532_STATUSBYTE_RDY));
  return true;
}

bool PN532::receiveAckFrame(uint8_t *data)
{
  /* Before initiating an exchange the host controller must write a byte indicating to the 
   * PN532 what the following operation is */
  Wire.beginTransmission(PN532Address); 
  Wire.write(PN532_FIRSTBYTE_DATAREAD);
  Wire.endTransmission();
  
  Wire.requestFrom(PN532Address, PN532_ACKFRAME_LENGTH);
  for (int i=0; i<6; i++)
  {
    while (!Wire.available()) ;
    data[i] = Wire.read();
#ifdef PN532_DEBUG
    Serial.print(" 0x"); Serial.print(data[i], HEX);
#endif
  }
#ifdef PN532_DEBUG
  Serial.println();
#endif
}

/** @}*/
