#include "J1939_EBC1.h"
#include "../inc/database.h"

/**
 * @brief  Parses PGN 0xF001 (EBC1 - Electronic Brake Controller 1).
 * @param  rxMsg: Pointer to received CAN message
 * @retval None
 */
void J1939_PGN_F001_EBC1(CanReceiver *rxMsg) {
  // PGN 61441 (0xF001) - EBC1
  // SPN 561 - ASR Engine Control Active (Byte 0, Bits 1-2)
  // SPN 563 - ABS Active (Byte 1, Bits 1-2)

  // 1. ASR Engine Control Active
  // Byte 0, Bits 0-1 (using mask 0x03)
  uint8_t asrEngineCtrl = rxMsg->data[0] & 0x03;
  DataBase_Set(db_EBC1_ASR_EngineControlActive, (float)asrEngineCtrl);

  // 2. ABS Active
  // Byte 1, Bits 0-1 (using mask 0x03)
  uint8_t absActive = rxMsg->data[1] & 0x03;
  DataBase_Set(db_EBC1_ABS_Active, (float)absActive);
}
