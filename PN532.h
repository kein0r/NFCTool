/** @ingroup PN532
 * @{
 */
#ifndef  PN532_H
#define  PN532_H

/*******************| Inclusions |*************************************/
#include <stdint.h>

/*******************| Macros |*****************************************/

#define PN532_DEBUG

/* Before initiating an exchange (either from the host controller to the PN532 or from the 
 * PN532 to the host controller), the host controller must write a byte indicating to the 
 * PN532 what the following operation is: 
 * First byte = xxxx xx10b status reading      (PN532 to host controller), 
 * First byte = xxxx xx01b data writing        (host controller to PN532), 
 * First byte = xxxx xx11b data reading        (PN532 to host controller). */
#define PN532_FIRSTBYTE_STATUSREAD        (uint8_t)0x02
#define PN532_FIRSTBYTE_DATEWRITE         (uint8_t)0x01
#define PN532_FIRSTBYTE_DATAREAD          (uint8_t)0x03

#define PN532_FRAME_PREAMBLE              (uint8_t)0x00
#define PN532_FRAME_STARTCODE1            (uint8_t)0x00
#define PN532_FRAME_STARTCODE2            (uint8_t)0xFF
#define PN532_FRAME_TFI_HOSTTOCONTROLLER  (uint8_t)0xD4
#define PN532_FRAME_TFI_CONTROLLERTOHOST  (uint8_t)0xD5
#define PN532_FRAME_POSTAMBLE             (uint8_t)0x00

#define PN532_ACKFRAME_LENGTH             (uint8_t)0x06
#define PN532_STATUSRAME_LENGTH           (uint8_t)0x01

#define PN532_STATUSBYTE_RDY              (uint8_t)0x01

/*******************| Type definitions |*******************************/
typedef enum {
  PN532_CommandCode_Diagnose = 0x00,
  PN532_CommandCode_GetFirmwareVersion = 0x02,
  PN532_CommandCode_GetGeneralStatus = 0x04
} PN532_CommandCode_t;

typedef uint8_t PN532_Data_t;

/**
 * Data structure used to exchange "Normal Information Frame" with PN532
 * Preamble (1 byte), startcode (2 bytes), and postamble are ommited here because they
 * are fix.
 * The command is separated from data into variable #command 
 * Checksum #packetLengthChecksum and #dataChecksum should not be filled by application
 * but calculated internally and therefore overwritten. 
 * Data pointer must be provided by application by seeting #data pointer accordingly *before* 
 * first communication. This way already existing application buffer can be re-used.
 */
typedef struct {
  uint8_t length;
  uint8_t packetLengthChecksum;
  uint8_t frameIdentifier;
  PN532_CommandCode_t command;
  PN532_Data_t* data;
  uint8_t dataChecksum;
} PN532_NormalInformationFrame_t;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

class PN532
{
private:
  const uint8_t PN532Address;
  uint8_t statusReady();
  
public:
  PN532(uint8_t address);
  bool sendCommand(PN532_CommandCode_t command, PN532_Data_t *data, uint8_t length);
  bool receiveAckFrame(uint8_t *data);
};

#endif

/** @}*/
