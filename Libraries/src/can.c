#include "project.h"
#include <stdio.h> // For proper types if needed, or stick to HAL types

// Typedef for internal clarity if needed, but we use CanReceiver from
// CANDrive.h

/**
 * @brief  Decodes Standard ID Messages.
 *         Static function local to this file.
 * @param  rxMsg: Pointer to received message
 * @retval None
 */
static void CANDecodeRxStd(CanReceiver *rxMsg) {
  switch (rxMsg->StdId) {
  case 0x123:
    // Example: Standard ID 0x123 Handler
    break;

  case 0x321:
    // Example: Echo functionality (moved from CANDrive example)
    CANDrive_Transmit(0x456, rxMsg->data, rxMsg->DLC, CAN_ID_STD);
    break;

  default:
    break;
  }
}

/**
 * @brief  Decodes Extended ID Messages (e.g., J1939).
 *         Static function local to this file.
 * @param  rxMsg: Pointer to received message
 * @retval None
 */
static void CANDecodeRxExt(CanReceiver *rxMsg) {
  switch (CANIDEXT_GETPGN(rxMsg->ExtId)) {
  case 0xFEE5:
    // J1939 Generic OEM Message
    J1939_PGN_FEE5_OEM(rxMsg);
    // Send response with different ID (Proprietary A)
    CANDrive_Transmit(0x18FFFA00, rxMsg->data, rxMsg->DLC, CAN_ID_EXT);
    break;

  case 0xFEF1: // CCVS1
    J1939_PGN_FEF1_CCVS1(rxMsg);
    break;

  case 0xF001: // EBC1
    J1939_PGN_F001_EBC1(rxMsg);
    break;

  default:
    // Default PGN Handler
    break;
  }
}

/**
 * @brief  Main entry point for processing received CAN messages.
 *         Dispatches to Standard or Extended decoders based on IDE.
 * @param  rxMsg: Pointer to received message
 * @retval None
 */
void CAN_ProcessMessage(CanReceiver *rxMsg) {
  if (rxMsg->IDE == CAN_ID_STD) {
    CANDecodeRxStd(rxMsg);
  } else {
    CANDecodeRxExt(rxMsg);
  }
}
