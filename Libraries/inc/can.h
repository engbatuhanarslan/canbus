#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "CANDrive.h"

// J1939 PGN Extraction Macro
// PGN is usually bits 8-25 of the 29-bit Identifier
// (This is a simplified view; J1939 PGN logic can be more complex with Data
// Page and PDU Format) For this request: (ID >> 8) & 0xFFFF is a common
// simplistic extraction for PDU1/PDU2 mixed. User requested:
// CANIDEXT_GETPGN(pMsg->id)
#define CANIDEXT_GETPGN(id) ((id >> 8) & 0xFFFF)

// Function Prototypes
void CAN_ProcessMessage(CanReceiver *rxMsg);

#endif /* INC_CAN_H_ */
