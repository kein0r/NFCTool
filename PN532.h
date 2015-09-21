/** @ingroup PN532
 * @{
 */
#ifndef  PN532_H
#define  PN532_H

/*******************| Inclusions |*************************************/
#include <stdint.h>

/*******************| Macros |*****************************************/

#define PN532_DEBUG

#define PN532_FRAME_PREAMBLE              (uint8_t)0x00
#define PN532_FRAME_STARTCODE1            (uint8_t)0x00
#define PN532_FRAME_STARTCODE2            (uint8_t)0xFF
#define PN532_FRAME_TFI_HOSTTOCONTROLLER  (uint8_t)0xD4
#define PN532_FRAME_TFI_CONTROLLERTOHOST  (uint8_t)0xD5
#define PN532_FRAME_POSTAMBLE             (uint8_t)0x00

#define PN532_FRAME_LENGTH                sizeof(PN532_NormalInformationFrame_t)
#define PN532_FRAMEHEADER_LENGTH          (uint8_t)0x06
#define PN532_FRAMEFOOTER_LENGTH          (uint8_t)0x02
#define PN532_ACKFRAME_LENGTH             sizeof(PN532_AckFrame_t)

#define PN532_STATUSRAME_LENGTH           (uint8_t)0x01

#define PN532_LOWBYTE(a)                  (uint8_t)(a&0x00ff)
#define PN532_HIGHBYTEBYTE(a)             (uint8_t)(a&0x00ff)

/**
 * Defines how often a status byte is requests and check for ready status
 * before giving up */
#define PN532_STATUSBYTE_TIMEOUT          (uint8_t)0x20
#define PN532_STATUSBYTE_RDY              (uint8_t)0x01

/*******************| Type definitions |*******************************/
/**
 * Struct to store all fix information, that is no data part, of a normal information
 * frame send or received to or from PN532
 */
typedef struct {
  uint8_t preamble;              /*!< fix 0x00 */
  uint16_t startcode;            /*!< fix 0x00ff */
  uint8_t length;                /*!< lenght of data plus one byte for TFI */
  uint8_t lengthChecksum;        /*!< checksum for length, #length + #lengthChecksum = 0 */
  uint8_t tfi;                   /*!< frame identifier, 0xd4 in case of frame from host to PN532, 0xd5 in case of frame from PN532 to host */
  uint8_t dataChecksum;          /*!< data checksum TFI + d0 + d1 + ... +dn = 0 */
  uint8_t postamble;             /*!< fix 0x00 */
} PN532_NormalInformationFrame_t;

typedef struct {
  uint8_t preamble;              /*!< fix 0x00 */
  uint16_t startOfPacketCode;    /*!< fix 0x00ff */
  uint16_t packetCode;           /*!< 0x0ff for ACK frame, 0xff00 for NACK frame */
  uint8_t postamble;             /*!< fix 0x00 */
} PN532_AckFrame_t;

typedef enum {
  PN532_CommandCode_Diagnose = 0x00,
  PN532_CommandCode_GetFirmwareVersion = 0x02,
  PN532_CommandCode_GetGeneralStatus = 0x04,
  PN532_CommandCode_ReadRegister = 0x06,
  PN532_CommandCode_WriteRegister = 0x08,
  PN532_CommandCode_ReadGPIO = 0x0c,
  PN532_CommandCode_WriteGPIO = 0x0e,
  PN532_CommandCode_SetSerialBaudRate = 0x10,
  PN532_CommandCode_SetParameters = 0x12,
  PN532_CommandCode_SAMConfiguration = 0x14,
  PN532_CommandCode_PowerDown = 0x16
} PN532_CommandCode_t;

typedef uint8_t PN532_Data_t;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

class PN532
{
private:
  const uint8_t PN532Address;
  void waitForStatusReady();
  bool receiveAckFrame();
  
public:
  PN532(uint8_t address);
  bool sendCommand(PN532_CommandCode_t command, PN532_Data_t *data, uint8_t length);
  uint8_t receiveResponse(PN532_Data_t *data);
};

#endif

/** @}*/
