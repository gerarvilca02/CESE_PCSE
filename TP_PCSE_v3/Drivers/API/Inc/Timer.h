#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

void Timer_Start(uint32_t *time);
bool Timer_Update(uint32_t *time);

#endif
