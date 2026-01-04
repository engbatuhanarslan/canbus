#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f4xx.h" // Assumes STM32F4 CMSIS/HAL is available

// Time Constants
#define TIMER_1MS 1
#define TIMER_100MS 100
#define TIMER_1S 1000

// Timer Modes
typedef enum { TIMER_MODE_ONESHOT, TIMER_MODE_PERIODIC } TimerMode;

// Struct for software timer (Non-blocking)
typedef struct {
  uint32_t startTime;
  uint32_t duration;
  uint8_t active;
  TimerMode mode;
} Timer;

// Function Prototypes
void Timer_Start(Timer *timer, uint32_t ms, TimerMode mode);
uint8_t Timer_HasExpired(Timer *timer);

#endif /* INC_TIMER_H_ */
