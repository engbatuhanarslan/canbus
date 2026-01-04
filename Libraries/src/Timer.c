#include "Timer.h"

/**
 * @brief  Starts a non-blocking software timer.
 * @param  timer: Pointer to Timer struct
 * @param  ms: Duration in milliseconds
 * @param  mode: Timer mode (TIMER_MODE_ONESHOT or TIMER_MODE_PERIODIC)
 * @retval None
 */
void Timer_Start(Timer *timer, uint32_t ms, TimerMode mode) {
  timer->startTime = HAL_GetTick();
  timer->duration = ms;
  timer->active = 1;
  timer->mode = mode;
}

/**
 * @brief  Checks if the timer has expired.
 * @param  timer: Pointer to Timer struct
 * @retval 1 if expired, 0 otherwise
 */
uint8_t Timer_HasExpired(Timer *timer) {
  if (timer->active) {
    if ((HAL_GetTick() - timer->startTime) >= timer->duration) {
      if (timer->mode == TIMER_MODE_PERIODIC) {
        // Auto-reload for periodic mode
        timer->startTime += timer->duration;
      } else {
        // One-shot mode: stop timer
        timer->active = 0;
      }
      return 1;
    }
  }
  return 0;
}
