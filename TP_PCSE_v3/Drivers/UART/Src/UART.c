#include "UART.h"

static uint8_t txBuffer[UART_BUFFER_SIZE];


bool_t UART_Terminal_Init(){
	UART_Init();

	// Limpieza del terminal
	uint8_t clearCommand[] = "\033[2J\033[H";
	HAL_UART_Transmit(&huart2, clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

	// Mostrar mensajes de bienvenida
	uint8_t welcomeMessage[] = "BIENVENIDO\r\n";
	HAL_UART_Transmit(&huart2, welcomeMessage, sizeof(welcomeMessage) - 1, HAL_MAX_DELAY);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart2, clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

	uint8_t initMessage[] = "SISTEMA DE RIEGO AUTOMÁTICO\r\n";
	HAL_UART_Transmit(&huart2, initMessage, sizeof(initMessage) - 1, HAL_MAX_DELAY);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart2, clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

	uint8_t presentacionMessage[] = "TP PCSE - Gerardo Vilcamiza\r\n";
	HAL_UART_Transmit(&huart2, presentacionMessage, sizeof(presentacionMessage) - 1, HAL_MAX_DELAY);
	HAL_Delay(1500);

	// Limpiar pantalla
	HAL_UART_Transmit(&huart2, clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

	return true;
}

void uartSendString(uint8_t *pstring){
    if (pstring != NULL){
        size_t len = strlen((const char*)pstring);
        if (len < UART_BUFFER_SIZE){
            memcpy(txBuffer, pstring, len);
            HAL_UART_Transmit(&huart2, txBuffer, len, HAL_MAX_DELAY);
        }
    }
}


