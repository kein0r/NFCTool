/** @ingroup PN532
 * @{
 */

/*******************| Inclusions |*************************************/
#include "PN532.h"
#include <TwoWirePlus.h>
#include <Arduino.h>

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
 * @return true if status byte with status bit ready set received in time, else false
*/
bool PN532::waitForStatusReady()
{
  uint8_t statusReadTimeout = PN532_STATUSBYTE_TIMEOUT;
  uint8_t statusByte = 0x00;
#ifdef PN532_DEBUG
    Serial.print("Wait");
#endif
  while ((!(statusByte & PN532_STATUSBYTE_RDY)) && statusReadTimeout)
  {
#ifdef PN532_DEBUG
    Serial.print(" .");
#endif
    /* Read status byte and wait until its fully received */
    Wire.requestFrom(PN532Address, 1);
    statusByte = Wire.read();
    statusReadTimeout--;
  }
  if (statusReadTimeout == 0)
  {
#ifdef PN532_DEBUG
    Serial.println(" Timeout!");
#endif
    return false;
  }
#ifdef PN532_DEBUG
  Serial.println(" Ready!");
#endif
  return true;  
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
  uint8_t checksum;
  
  Wire.beginTransmission(PN532Address);
  /* Before initiating an exchange the host controller must write a byte indicating to the 
   * PN532 what the following operation is */
  //Wire.write(PN532_FIRSTBYTE_DATEWRITE);

  /* Send fix header */  
  Wire.write(PN532_FRAME_PREAMBLE);
  Wire.write(PN532_FRAME_STARTCODE1);
  Wire.write(PN532_FRAME_STARTCODE2);

  /* length value sent to PN352 should include TFI and command byte therefore we add two bytes to LCS. */
  Wire.write((uint8_t)(length + 2));
  /* Send length checksum */
  Wire.write((uint8_t)~(length + 2) + 1);
  
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
  /* Needed due to "However, it may happen that the PN532 does not acknowledge its 
   * own address immediately after having finished a previous exchange." (Datasheet
   * Chapter I2C communication details) therefore we wait 1ms. */
  delay(1);
  if (waitForStatusReady())
  {
    receiveAckFrame();
    return true;
  }
  return false;
}


/**
 * Receives ACK/NACK from from PN532.
 * @pre Wire.beginReception(address) was called and PN532 is in ready state (i.e. ready bit was received).
 * @return true if ACK frame was received, else false
 */
bool PN532::receiveAckFrame()
{
  uint8_t data[PN532_ACKFRAME_LENGTH];
#ifdef PN532_DEBUG 
  Serial.println("ACK/NACK: ");
#endif
  Wire.requestFrom(PN532Address, PN532_ACKFRAME_LENGTH + 1);
  /* Ignore first byte. Somehow PN532 sends ready bit again */
  data[0] = Wire.read();
  for (int i=0; i<PN532_ACKFRAME_LENGTH; i++)
  {
    data[i] = Wire.read();
#ifdef PN532_DEBUG
    Serial.print(" 0x"); Serial.print(data[i], HEX);
#endif
  }
#ifdef PN532_DEBUG
  Serial.println("!");
#endif
  return true;
}

/**
 * Receives response from PN532. Number of bytes to receive are determined by the
 * length field in normal information frame header and therefore depend on the command
 * send to PN532 earlier. Application must ensure that there is enough space in 
 * buffer to store the response.
 * @param data Pointer to bufffer where to store the response
 * @param length Number of bytes to receive
 * @return Number of bytes provided length byte in frame. Zero in case of CRC (length of data) error. 
 * This can be more bytes than actual requested by #length parameter. In this case only #length bytes
 * were read.
 * @note Only normal information frames are handled right now. Extended information
 * frame will give length checksum error.
 */
uint8_t PN532::receiveResponse(PN532_Data_t *data, uint8_t length)
{
  uint8_t checksum;
  uint8_t validDataLength;    /* Number of valid data bytes as reported by length byte */
  
  /* Wait until PN532 is ready to send response */
  waitForStatusReady();
  Wire.beginReception(PN532Address);
  Wire.requestBytes(PN532_FRAME_LENGTH + 1 + length);
  /* Wait until header is completely received to read it */
  while (Wire.available() < PN532_FRAMEHEADER_LENGTH + 1 ) ;
  /* Ignore first byte. PN532 sends status byte again. However we assume that PN532 is
   * in ready state. */
  Wire.read();
  Wire.read();  /* Preamble */
  Wire.read();  /* First byte startcode */
  Wire.read();  /* Second byte startcode */
  validDataLength = Wire.read(); /* Data length */
  checksum = validDataLength + Wire.read();  /* Date length checksum */
  /* Adjust validDataLength because it right now also contains TFI byte */
  validDataLength--;
  /* In case of length checksum error we set length to 0 */
  if ( checksum != 0x00)
  {
    validDataLength = 0;
  }
  /* @todo Check for extended frame if length and length checksum are 0xff */
  checksum = Wire.read();  /* TFI */
  for (int i=0; i< length; i++)
  {
    /* Wait until some data is available and read it from buffer */
    while (! Wire.available()) ;
    data[i] = Wire.read();
    checksum += data[i];
  }
  /* Only two more bytes to read. We request end of communication now */
  Wire.endReception();
  checksum += Wire.read();
  Wire.read();  /* Postamble */
  /* Check if crc matched and return 0 valid bytes if not */
  if (checksum != 0x00)
  {
    validDataLength = 0;
  }
  return validDataLength;
}

/** @}*/
