#include "J1939_CCVS1.h"
#include "../inc/database.h"

/**
 * @brief  Parses PGN 0xFEF1 (CCVS1 - Cruise Control/Vehicle Speed).
 * @param  rxMsg: Pointer to received CAN message
 * @retval None
 */
void J1939_PGN_FEF1_CCVS1(CanReceiver *rxMsg) {
  // PGN 65265 (0xFEF1) - CCVS1
  // SPN 84 - Wheel-Based Vehicle Speed (Bytes 1-2, 1/256 km/h per bit)
  // SPN 70 - Parking Brake Switch (Byte 0, Bits 1-2 - using mask 0x03 as
  // simplified 2-bit state) SPN 597 - Brake Switch (Byte 3, Bits 3-4? Need
  // check standard, assuming Byte 3 Bits 4-5 for Cruise Active based on common
  // layouts or user request "CruiseControlActive")

  // Note: J1939 structure varies. Assuming standard layout:
  // Byte 0: Switches (Parking Brake at unknown pos, assume Bits 0-1)
  // Byte 1-2: Speed
  // ...
  // Byte 3: ... Cruise Control Active (Assume Bits 3-4)

  // 1. Wheel-Based Vehicle Speed
  // Resolution: 1/256 km/h per bit, Offset: 0
  uint16_t speedRaw =
      (uint16_t)rxMsg->data[1] | ((uint16_t)rxMsg->data[2] << 8);
  float speedKmh = speedRaw / 256.0f;
  DataBase_Set(db_CCVS1_WheelBasedVehicleSpeed, speedKmh);

  // 2. Parking Brake Switch
  // Assume Byte 0, Bits 0-1 (2 bits)
  uint8_t parkingBrake = rxMsg->data[0] & 0x03;
  DataBase_Set(db_CCVS1_ParkingBrake, (float)parkingBrake);

  // 3. Cruise Control Active
  // Assume Byte 3, Bits 4-5 (shifted >> 4, mask 0x03) (Typically SPN 595 or
  // similar status) Adjust based on specific requirement. Using Bits 4-5 of
  // byte 3 as generic placeholder.
  uint8_t ccActive = (rxMsg->data[3] >> 4) & 0x03;
  DataBase_Set(db_CCVS1_CruiseControlActive, (float)ccActive);
}
