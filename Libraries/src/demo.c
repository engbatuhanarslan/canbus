#include "../inc/demo.h"
#include "../inc/database.h"
#include <stddef.h> // For NULL

#include "../inc/CANDrive.h"

// Static variable to hold the database element
static DBELEMENT_T x;

void Demo_Update(void) {
  // Check for timeout (1000ms) - Invalidates signal if stale
  DataBase_CanMessageTimeoutCheck(db_FEE5_TestSignal, 1000);

  // Retrieve the value for signal db_FEE5_TestSignal
  // The third argument corresponds to 'unused' in the prototype
  DataBase_Get(&x, db_FEE5_TestSignal, 0);

  // Check condition: If data is valid and value is 1 (Example)
  if (x.IsValid && x.Data.u32 == 1) {
    uint8_t txData[8] = {0};

    // "0. byte .bit" - Setting Bit 0 of Byte 0
    txData[0] = 0x01;

    // Send Extended ID Message (IDE = 1)
    // Example ID: 0x18F00500 (Extended)
    CANDrive_Transmit(0x18F00500, txData, 8, 1);
  }
}
