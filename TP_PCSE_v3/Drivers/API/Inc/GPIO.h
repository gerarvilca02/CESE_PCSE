#ifndef GPIO_H
#define GPIO_H

#include "stm32f4xx_hal.h"

void GPIO_Init(void);
void LED_Bomba_On(void);
void LED_Bomba_Off(void);
void LED_Alerta_On(void);
void LED_Alerta_Off(void);
uint8_t Leer_Pulsador_Humedad(void);
uint8_t Leer_Pulsador_PIR(void);
uint8_t Leer_Pulsador_LDR(void);

#endif
