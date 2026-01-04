#ifndef INC_CANDRIVE_H_
#define INC_CANDRIVE_H_

#include "stm32f4xx.h" // Assumes STM32F4 CMSIS/HAL is available
#include <stdint.h>

// CAN Configuration Constants
#define CAN_BAUDRATE_500KBPS // Default configuration

// Struct for received messages (User Requested Format)
typedef struct {
  uint32_t StdId;  // Standard Identifier
  uint32_t ExtId;  // Extended Identifier
  uint32_t IDE;    // Identifier Extension (0 = Standard, 1 = Extended)
  uint32_t RTR;    // Remote Transmission Request
  uint32_t DLC;    // Data Length Code (0-8)
  uint8_t data[8]; // Data payload: data[0]..data[7]
} CanReceiver;

// Function Prototypes
void CANDrive_Init(void);
void CANDrive_Transmit(uint32_t id, uint8_t *pData, uint8_t len, uint32_t ide);
uint8_t CANDrive_Receive(
    CanReceiver *rxMsg); // Returns 1 if message received, 0 otherwise
void CANDrive_OnReceive(
    CanReceiver *rxMsg); // Callback for interrupt-driven reception

#endif /* INC_CANDRIVE_H_ */
