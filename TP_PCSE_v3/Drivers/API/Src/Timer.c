#include "Timer.h"


void Timer_Start(uint32_t *time){
    *time = HAL_GetTick();  // Guarda el tiempo de inicio
}

bool Timer_Update(uint32_t *time){
    uint32_t currentTime = HAL_GetTick();
    if ((currentTime - *time) >= 1000){  // Cada 1 segundo
        (*time)--;
        Timer_Start(time);  // Reinicia el tiempo de inicio
        if (*time == 0){
            return true;  // El tiempo terminó
        }
    }
    return false;
}
