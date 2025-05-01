#include "GPIO.h"
#include "API_delay.h"

static delay_t delayDebounce;

static uint8_t estadoHumedad = 1;
static uint8_t estadoPIR = 0;
static uint8_t estadoLDR = 1;
static uint8_t lastStateHumedad = 1;
static uint8_t lastStatePIR = 1;
static uint8_t lastStateLDR = 1;


void GPIO_Init(void){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configuración LED de la bomba de agua
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configuración LED de alerta
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configuración pulsadores
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Inicializar delay para antirrebote
    delayInit(&delayDebounce, 50);  // Retardo de 50 ms
}

void LED_Bomba_On(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);  // Encender LED bomba
}

void LED_Bomba_Off(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  // Apagar LED bomba
}

void LED_Alerta_On(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);  // Encender LED alerta
}

void LED_Alerta_Off(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);  // Apagar LED alerta
}

uint8_t Leer_Pulsador_Humedad(void){
    uint8_t currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    if (currentState == GPIO_PIN_SET && lastStateHumedad == GPIO_PIN_RESET){
        if (delayRead(&delayDebounce)){  // Lógica antirrebote
            estadoHumedad = !estadoHumedad;  // Cambia el estado cuando se suelta el botón
        }
    }
    lastStateHumedad = currentState;
    return estadoHumedad;
}

uint8_t Leer_Pulsador_PIR(void){
    uint8_t currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
    if (currentState == GPIO_PIN_SET && lastStatePIR == GPIO_PIN_RESET){
        if (delayRead(&delayDebounce)){
            estadoPIR = !estadoPIR;
        }
    }
    lastStatePIR = currentState;
    return estadoPIR;
}

uint8_t Leer_Pulsador_LDR(void){
    uint8_t currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
    if (currentState == GPIO_PIN_SET && lastStateLDR == GPIO_PIN_RESET){
        if (delayRead(&delayDebounce)){
            estadoLDR = !estadoLDR;
        }
    }
    lastStateLDR = currentState;
    return estadoLDR;
}
