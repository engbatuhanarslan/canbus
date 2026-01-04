#include "../inc/database.h"
#include "stm32f4xx.h" // For HAL_GetTick
#include <stdint.h>
#include <string.h> // For memset if needed

// Static storage for database signals
static DBELEMENT_T db_table[DB_MAX_SIGNALS];

/**
 * @brief  Sets a signal value in the database.
 *         Currently supports writing as float, can be expanded for other types.
 * @param  sigId: Signal ID
 * @param  val: Value to set (float)
 * @retval None
 */
void DataBase_Set(DB_SIGNAL_ID_T sigId, float val) {
  if (sigId < DB_MAX_SIGNALS) {
    db_table[sigId].PreviousTime = db_table[sigId].CurrentTime;
    db_table[sigId].CurrentTime = HAL_GetTick();
    db_table[sigId].Data.flt = val;
    db_table[sigId].IsValid = 1;
    // Note: If you need to store as u32/i32, you might need separate Set
    // functions or passed a union type. For now, assuming float updates as per
    // request.
  }
}

/**
 * @brief  Checks if a specific signal has timed out.
 * @param  sigId: Signal ID to check
 * @param  ms: Timeout duration in milliseconds
 * @retval true if timed out (or invalid), false if valid and within timeout
 */
bool DataBase_CanMessageTimeoutCheck(DB_SIGNAL_ID_T sigId, uint32_t ms) {
  if (sigId >= DB_MAX_SIGNALS) {
    return true; // Invalid ID treated as timeout/error
  }

  uint32_t currentTick = HAL_GetTick();

  // If already invalid, considered timed out (or never received)
  if (!db_table[sigId].IsValid) {
    return true;
  }

  // Check timeout
  if ((currentTick - db_table[sigId].CurrentTime) > ms) {
    db_table[sigId].IsValid = 0; // Invalidate
    return true;                 // Timed out
  }

  return false; // Valid and within time
}

/**
 * @brief  Checks if a signal has been received (valid).
 * @param  sigId: Signal ID
 * @retval 1 if valid, 0 if invalid
 */
uint8_t DataBase_IsValidItem(DB_SIGNAL_ID_T sigId) {
  if (sigId < DB_MAX_SIGNALS) {
    return db_table[sigId].IsValid;
  }
  return 0;
}

/**
 * @brief  Gets a signal value from the database.
 * @param  pOut: Pointer to output DBELEMENT_T structure
 * @param  sigId: Signal ID
 * @param  unused: Reserved parameter
 * @retval None
 */
void DataBase_Get(DBELEMENT_T *pOut, DB_SIGNAL_ID_T sigId, uint8_t unused) {
  if (sigId < DB_MAX_SIGNALS && pOut != 0) {
    *pOut = db_table[sigId];
  }
}
