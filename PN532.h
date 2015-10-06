/** @ingroup PN532
 * @{
 */
#ifndef  PN532_H
#define  PN532_H

/*******************| Inclusions |*************************************/
#include <stdint.h>

/*******************| Macros |*****************************************/

//#define PN532_DEBUG

#define PN532_FRAME_PREAMBLE              (uint8_t)0x00
#define PN532_FRAME_STARTCODE1            (uint8_t)0x00
#define PN532_FRAME_STARTCODE2            (uint8_t)0xFF
#define PN532_FRAME_TFI_HOSTTOCONTROLLER  (uint8_t)0xD4
#define PN532_FRAME_TFI_CONTROLLERTOHOST  (uint8_t)0xD5
#define PN532_FRAME_POSTAMBLE             (uint8_t)0x00

#define PN532_FRAMEHEADER_LENGTH          (uint8_t)0x06
#define PN532_FRAMEFOOTER_LENGTH          (uint8_t)0x02
#define PN532_FRAME_LENGTH                PN532_FRAMEHEADER_LENGTH + PN532_FRAMEFOOTER_LENGTH
#define PN532_ACKFRAME_LENGTH             sizeof(PN532_AckFrame_t)

#define PN532_STATUSRAME_LENGTH           (uint8_t)0x01

#define PN532_LOWBYTE(a)                  (uint8_t)(a&0x00ff)
#define PN532_HIGHBYTEBYTE(a)             (uint8_t)(a&0x00ff)

/**
 * Defines how often a status byte is requests and check for ready status
 * before giving up */
#define PN532_STATUSBYTE_TIMEOUT          (uint8_t)0x10
#define PN532_STATUSBYTE_RDY              (uint8_t)0x01

/*******************| Type definitions |*******************************/

typedef struct {
  uint8_t preamble;              /*!< fix 0x00 */
  uint16_t startOfPacketCode;    /*!< fix 0x00ff */
  uint16_t packetCode;           /*!< 0x0ff for ACK frame, 0xff00 for NACK frame */
  uint8_t postamble;             /*!< fix 0x00 */
} PN532_AckFrame_t;

typedef enum {
  /* Miscellaneous Communication Commands */
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
  PN532_CommandCode_PowerDown = 0x16,
  /* RF Communication Commands */
  PN532_CommandCode_RFConfiguration = 0x32,
  PN532_CommandCode_RFRegulationTest = 0x58,
  PN532_CommandCode_InJumpForDEP = 0x56,
  PN532_CommandCode_InJumpForPSL = 0x46,
  PN532_CommandCode_InListPassiveTarget = 0x4a,
  PN532_CommandCode_InATR = 0x50,
  PN532_CommandCode_InPSL = 0x4e,
  PN532_CommandCode_InDataExchange = 0x40,
  PN532_CommandCode_InCommunicateThru = 0x42,
  PN532_CommandCode_InDeselect = 0x44,
  PN532_CommandCode_InRelease = 0x52,
  PN532_CommandCode_InSelect = 0x54,
  PN532_CommandCode_InAutoPoll = 0x60,
  PN532_CommandCode_TgInitAsTarget = 0x8c,
  PN532_CommandCode_TgSetGeneralBytes = 0x92,
  PN532_CommandCode_TgGetData = 0x86,
  PN532_CommandCode_TgSetData = 0x8e,
  PN532_CommandCode_TgSetMetaData = 0x94,
  PN532_CommandCode_TgGetInitiatorCommand = 0x88,
  PN532_CommandCode_TgResponseToInitiatior = 0x90,
  PN532_CommandCode_TgSetTargetStatus = 0x8a
} PN532_CommandCode_t;

typedef uint8_t PN532_Data_t;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

class PN532
{
private:
  const uint8_t PN532Address;
  bool waitForStatusReady();
  bool receiveAckFrame();
  
public:
  PN532(uint8_t address);
  bool sendCommand(PN532_CommandCode_t command, PN532_Data_t *data, uint8_t length);
  uint8_t receiveResponse(PN532_Data_t *data, uint8_t length);
};

#endif

/** @}*/
