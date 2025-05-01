#include "main.h"
#include "UART.h"
#include "LCD_I2C.h"
#include "API_ADC.h"
#include "API_delay.h"
#include "GPIO.h"
#include "Timer.h"
#include <stdio.h>

ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

delay_t delayPausa;
delay_t delayRiego;

uint32_t tiempoRiegoInicio;
uint32_t tiempoRiegoSegundos;
uint32_t tiempoRestanteRiego;
uint32_t tiempoGuardadoRiego;
uint32_t adcValue;
uint8_t percentage;

char message[100];
static char lcdBuffer[17];

typedef enum {
    ESTADO_INICIO,
    ESTADO_MONITOREO,
    ESTADO_RIEGO,
    ESTADO_PAUSA,
    ESTADO_ALERTA_BAJA,
    ESTADO_ALERTA_CRITICA
} estado_t;

estado_t estadoActual = ESTADO_INICIO;


void iniciarSistema(){
    ADC_Init();
    UART_Terminal_Init();
    GPIO_Init();

    delayInit(&delayRiego, 1000); // Riego controlado por delay de 1 segundo

    LCD_I2C_Init();
    LCD_Clear();
    LCD_SetCursor(0,0);
    LCD_PrintString("BIENVENIDO");
    LCD_SetCursor(1,0);
    LCD_PrintString("Gerardo Vilcamiza");

    // Mover el cursor a la parte superior izquierda
    sprintf(message, "\033[H");
    uartSendString((uint8_t*)message);

    // Mensajes iniciales estáticos
    sprintf(message, "Sensor de Humedad: Húmedo   \r\n");
    uartSendString((uint8_t*)message);

    sprintf(message, "Sensor PIR: Sin presencia   \r\n");
    uartSendString((uint8_t*)message);

    sprintf(message, "Sensor LDR: Día   \r\n");
    uartSendString((uint8_t*)message);

    sprintf(message, "Capacidad del tanque: 0%%, Presión: 0 Pa\r\n");
    uartSendString((uint8_t*)message);
}

void manejarEstadoInicio(){
    iniciarSistema();
    estadoActual = ESTADO_MONITOREO;
}

void manejarEstadoRiego(){
    // Verifica si se detecta una persona o si el nivel de agua es crítico
    uint8_t estadoPIR = Leer_Pulsador_PIR();
    if (estadoPIR == 1){
        estadoActual = ESTADO_PAUSA;
        tiempoRestanteRiego = tiempoRiegoSegundos;  // Guarda el tiempo restante

        return;
    } else if (percentage <= 10){
        estadoActual = ESTADO_ALERTA_CRITICA;
        tiempoGuardadoRiego = tiempoRiegoSegundos;  // Guarda el tiempo restante en caso de alerta crítica

        return;
    } else if (percentage <= 20 && percentage > 10){
        // Estado de alerta baja, no se detiene el riego, pero se muestra el mensaje
        sprintf(message, "\033[6HAlerta: Nivel bajo de agua!  \r\n");
        uartSendString((uint8_t*)message);
    } else{
    	sprintf(message, "\033[6HTanque con nivel de agua óptimo \r\n");
		uartSendString((uint8_t*)message);
    }

    // Continúa la cuenta regresiva si no está en pausa y no hay condiciones críticas
    if (delayRead(&delayRiego)) {
        if (tiempoRiegoSegundos > 0) {
            tiempoRiegoSegundos--;
            // Actualiza la cuenta regresiva en la terminal
            sprintf(message, "\033[5HRegando... Tiempo restante: %d min   ", tiempoRiegoSegundos);
            uartSendString((uint8_t*)message);

            // Reinicia el temporizador para el próximo segundo
            delayWrite(&delayRiego, 1000);
        } else {
            // Si el tiempo de riego ha terminado
            LED_Bomba_Off();
            estadoActual = ESTADO_MONITOREO;
        }
    }

    // Lee y actualiza los estados de los sensores mientras se riega
    manejarEstadoMonitoreo();
}

void manejarEstadoMonitoreo(){
    // Lee y actualizar estados de los sensores
    uint8_t estadoHumedad = Leer_Pulsador_Humedad();
    uint8_t estadoPIR = Leer_Pulsador_PIR();
    uint8_t estadoLDR = Leer_Pulsador_LDR();

    // Actualiza los mensajes según los valores actuales
    sprintf(message, "\033[H\033[19C%s", estadoHumedad ? "Húmedo   " : "Seco     ");
    uartSendString((uint8_t*)message);

    sprintf(message, "\033[2H\033[12C%s", estadoPIR ? "Persona detectada!" : "Sin presencia      ");
    uartSendString((uint8_t*)message);

    sprintf(message, "\033[3H\033[12C%s", estadoLDR ? "Día      " : "Noche    ");
    uartSendString((uint8_t*)message);

    // Lee ADC para el nivel del tanque
    adcValue = ADC_Read();
    percentage = (adcValue * 100) / 4095;
    int altura_cm = percentage * 3;  // Simulación de altura en cm
    int presion_Pa = altura_cm * 98;

    // Actualiza la capacidad del tanque y la presión
    sprintf(message, "\033[4H\033[21C%3d%%, Presión: %4d Pa", percentage, presion_Pa);
    uartSendString((uint8_t*)message);

    LCD_SetCursor(0,0);
    snprintf(lcdBuffer, sizeof(lcdBuffer), "%3d%% %4dPa", percentage, presion_Pa);
    LCD_PrintString(lcdBuffer);
    LCD_SetCursor(1,0);
    LCD_PrintString(estadoLDR ? "Tpo:Dia   " : "Tpo:Noche ");

    // Verifica transiciones, pero sin interrumpir el estado de riego
    if (estadoActual != ESTADO_RIEGO){
        if (estadoHumedad == 0) {
            estadoActual = ESTADO_RIEGO;  // Si el suelo está seco, pasar al estado de riego
            tiempoRiegoSegundos = estadoLDR ? 30 : 15;  // Reinicia tiempo de riego según sea día o noche
            tiempoRestanteRiego = 0;
            tiempoGuardadoRiego = 0;
            tiempoRiegoInicio = HAL_GetTick();  // Guarda el tiempo de inicio del riego
            LED_Bomba_On();
        } else if (percentage <= 20 && percentage > 10) {
            estadoActual = ESTADO_ALERTA_BAJA;
        } else if (percentage <= 10) {
            estadoActual = ESTADO_ALERTA_CRITICA;
        }
    }
}

void manejarEstadoPausa(){
	LED_Bomba_Off();
    // Actualiza el mensaje indicando que el sistema está en pausa por detección de persona
    sprintf(message, "\033[5HPausa por detección de persona...\r\n");
    uartSendString((uint8_t*)message);

    // Lee el estado del PIR para ver si la persona ya se ha ido
    uint8_t estadoPIR = Leer_Pulsador_PIR();
    if (estadoPIR == 0) {  // Si la persona ya no está presente
        estadoActual = ESTADO_RIEGO;
        // Continúa desde el tiempo restante guardado
        tiempoRiegoSegundos = tiempoRestanteRiego;
        LED_Bomba_On();  // Reanuda la bomba
        delayWrite(&delayRiego, 1000);  // Continua el temporizador de cuenta regresiva
    }

    // Continúa actualizando los valores de los sensores en la terminal durante la pausa
    manejarEstadoMonitoreo();
}

void manejarEstadoAlertaCritica(){
	LED_Bomba_Off();  // Apaga la bomba en caso de alerta crítica
    sprintf(message, "\033[6HAlerta: Nivel crítico de agua!\r\n");
    uartSendString((uint8_t*)message);
    LCD_SetCursor(1,9);
    LCD_PrintString("ALERTA!");
    LED_Alerta_On();  // Enciende el LED de alerta

    // Permite la actualización del monitoreo durante la alerta
    manejarEstadoMonitoreo();

    // Reanuda el riego si el nivel sube por encima del 10%
    if (percentage > 10) {
        LED_Alerta_Off();
        estadoActual = ESTADO_RIEGO;
        // Continúa desde el tiempo guardado
        tiempoRiegoSegundos = tiempoGuardadoRiego;
        LED_Bomba_On();
        delayWrite(&delayRiego, 1000);  // Continúa el temporizador de cuenta regresiva
    }
}

void manejarEstadoAlertaBaja(){
    sprintf(message, "\033[6HAlerta: Nivel bajo de agua!\r\n");
    uartSendString((uint8_t*)message);
    LED_Alerta_On();  // Enciende el LED de alerta

    // Permite la actualización del monitoreo durante la alerta
    manejarEstadoMonitoreo();

    // Verifica si el nivel baja a crítico o vuelve a un nivel seguro
    if (percentage > 20) {
    	sprintf(message, "\033[6HTanque con nivel óptimo \r\n");
    	uartSendString((uint8_t*)message);
        estadoActual = ESTADO_RIEGO;  // Continúa con el riego
    } else if (percentage <= 10) {
        tiempoGuardadoRiego = tiempoRiegoSegundos;  // Guarda el tiempo restante en caso de alerta crítica
        estadoActual = ESTADO_ALERTA_CRITICA;
    }
}

void loop() {
	// Selección de estados
    switch (estadoActual){
        case ESTADO_INICIO:
            manejarEstadoInicio();
            break;
        case ESTADO_MONITOREO:
            manejarEstadoMonitoreo();
            break;
        case ESTADO_RIEGO:
            manejarEstadoRiego();
            break;
        case ESTADO_PAUSA:
            manejarEstadoPausa();
            break;
        case ESTADO_ALERTA_BAJA:
            manejarEstadoAlertaBaja();
            break;
        case ESTADO_ALERTA_CRITICA:
            manejarEstadoAlertaCritica();
            break;
    }
    HAL_Delay(100);
}


int main(void){
  HAL_Init();
  SystemClock_Config();

  while (1){
	  loop();
  }
}


void SystemClock_Config(void){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
    Error_Handler();
  }
}


void Error_Handler(void){
  __disable_irq();
  while (1){}
}

