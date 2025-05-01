#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

#define UART_BUFFER_SIZE 64  // Tamaño del buffer

typedef bool bool_t;

bool_t UART_Init();
bool_t UART_Terminal_Init();
void uartSendString(uint8_t *pstring);
void uartSendStringSize(uint8_t *pstring, uint16_t size);
void uartReceiveStringSize(uint8_t *pstring, uint16_t size);

#endif
