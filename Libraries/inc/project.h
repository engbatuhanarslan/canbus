#ifndef INC_PROJECT_H_
#define INC_PROJECT_H_

/**
 * @brief Central Project Header
 *        Includes all custom libraries and modules for the application.
 */

// Core Libraries
#include "CANDrive.h"
#include "Timer.h"

// High-Level CAN & Database
#include "can.h"
#include "database.h"

// Application Demo
#include "demo.h"

// J1939 Modules
#include "../J1939/J1939_CCVS1.h"
#include "../J1939/J1939_EBC1.h"
#include "../J1939/J1939_FEE5_OEM.h"


#endif /* INC_PROJECT_H_ */
