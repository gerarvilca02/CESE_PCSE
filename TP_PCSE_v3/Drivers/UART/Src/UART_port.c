#include "UART.h"

static uint8_t txBuffer[UART_BUFFER_SIZE];
static uint8_t rxBuffer[UART_BUFFER_SIZE];


bool_t UART_Init(){
	__HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK){
        return false;
    }

    return true;
}

void uartSendStringSize(uint8_t *pstring, uint16_t size){
    if (pstring != NULL && size > 0 && size <= UART_BUFFER_SIZE){
        memcpy(txBuffer, pstring, size);
        HAL_UART_Transmit(&huart2, txBuffer, size, HAL_MAX_DELAY);
    }
}

void uartReceiveStringSize(uint8_t *pstring, uint16_t size){
    if (pstring != NULL && size > 0 && size <= UART_BUFFER_SIZE){
        HAL_UART_Receive(&huart2, rxBuffer, size, HAL_MAX_DELAY);
        memcpy(pstring, rxBuffer, size);
    }
}
