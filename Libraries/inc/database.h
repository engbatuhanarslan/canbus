#ifndef INC_DATABASE_H_
#define INC_DATABASE_H_

#include <stdbool.h>
#include <stdint.h>


// Union to hold various data types
typedef union {
  float flt;
  uint32_t u32;
  int32_t i32;
  uint8_t u8;
} DB_VALUE_U;

// Database Element Structure
typedef struct {
  DB_VALUE_U Data;
  uint8_t IsValid; // 0 = Invalid/Not Received, 1 = Valid/Received
  uint32_t CurrentTime;
  uint32_t PreviousTime;
} DBELEMENT_T;

// Signal IDs
typedef enum {
  db_WheelBasedVehicleSpeed_84, // Example Signal
  db_FEE5_TestSignal,           // Example J1939 Signal
  // CCVS1 Signals
  db_CCVS1_WheelBasedVehicleSpeed,
  db_CCVS1_ParkingBrake,
  db_CCVS1_CruiseControlActive,
  // EBC1 Signals
  db_EBC1_ASR_EngineControlActive,
  db_EBC1_ABS_Active,
  // Add more signals here
  DB_MAX_SIGNALS
} DB_SIGNAL_ID_T;

// Function Prototypes
void DataBase_Set(DB_SIGNAL_ID_T sigId, float val);
void DataBase_Get(DBELEMENT_T *pOut, DB_SIGNAL_ID_T sigId, uint8_t unused);
uint8_t DataBase_IsValidItem(DB_SIGNAL_ID_T sigId);
bool DataBase_CanMessageTimeoutCheck(DB_SIGNAL_ID_T sigId, uint32_t ms);

#endif /* INC_DATABASE_H_ */
