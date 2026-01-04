#include "J1939_FEE5_OEM.h"
#include "../inc/database.h" // For updating database

/**
 * @brief  Parses PGN 0xFEE5 (OEM Generic Message).
 *         Example: Extracting a float value from bytes 0-3 (Little Endian).
 * @param  rxMsg: Pointer to received CAN message
 * @retval None
 */
void J1939_PGN_FEE5_OEM(CanReceiver *rxMsg) {
  // Example Parsing:
  // Assume Bytes 0-3 represent a float value (IEEE 754)
  // Assume Bytes 4-7 are unused or another signal

  // 1. Combine bytes into a raw buffer or union for casting
  DB_VALUE_U tempVal;

  // Assuming Little Endian (Intel Byte Order)
  tempVal.u32 = (uint32_t)rxMsg->data[0] | ((uint32_t)rxMsg->data[1] << 8) |
                ((uint32_t)rxMsg->data[2] << 16) |
                ((uint32_t)rxMsg->data[3] << 24);

  // 2. Update Database
  // Using the new signal enum: db_FEE5_TestSignal
  DataBase_Set(db_FEE5_TestSignal, tempVal.flt);
}
