# Sistema de Riego Automático Embebido

## Descripción del proyecto
Este proyecto implementa un sistema de riego automático basado en la placa STM32 NUCLEO-F411RE. Su objetivo es optimizar el uso del agua y mantener la humedad del suelo dentro de niveles adecuados, adaptándose automáticamente al momento del día (día/noche) y deteniendo el riego si detecta personas o niveles bajos/ críticos de agua. Incluye dos interfaces de visualización (LCD I²C en campo y terminal UART en la sala de control) y múltiples mecanismos de seguridad.

---

## Drivers y su intención / funcionalidad

### 1. UART  
**Archivos:** `UART.c` · `UART_port.c`  
- **Intención:** Proveer comunicación bidireccional con una terminal serial para una pantalla de supervisión remota.  
- **Funcionalidad principal:**  
  - Inicialización de USART2 a 115200 bps.  
  - Envío de cadenas con posicionamiento de cursor (ANSI).  
  - Recepción y transmisión de bloques de datos de tamaño configurable.

### 2. LCD I²C  
**Archivos:** `LCD_I2C.c` · `LCD_I2C_port.c`  
- **Intención:** Mostrar datos de estado en una pantalla LCD 16×2 de campo usando un expansor I²C.  
- **Funcionalidad principal:**  
  - Secuencia de arranque del módulo en modo 4-bits.  
  - Limpieza de pantalla y posicionamiento de cursor.  
  - Impresión de cadenas y definición de caracteres personalizados.  
  - Abstracción de los pulsos de Enable y manejo de backlight.

### 3. API_ADC  
**Archivo:** `API_ADC.c`  
- **Intención:** Leer el nivel simulado de agua del tanque a través de un canal analógico.  
- **Funcionalidad principal:**  
  - Configuración de ADC a 12 bits y prescaler PCLK/4.  
  - Arranque de conversión, espera de finalización y lectura de valor bruto.  
  - Cálculo de porcentaje de llenado a partir de la lectura.

### 4. API_delay  
**Archivo:** `API_delay.c`  
- **Intención:** Gestionar retardos no bloqueantes para temporizar riego y pausas.  
- **Funcionalidad principal:**  
  - Inicializar objetos `delay_t` con duración configurable.  
  - Lectura de expiración sin detener el flujo de ejecución.  
  - Actualización dinámica del periodo de delay.

### 5. GPIO  
**Archivo:** `GPIO.c`  
- **Intención:** Leer pulsadores/sensores digitales y controlar salidas (LEDs, bomba).  
- **Funcionalidad principal:**  
  - Configuración de pines de entrada con antirrebote (humedad, PIR, LDR).  
  - Encendido/apagado de LEDs de estado y de la bomba.  
  - Lectura de flancos y mantenimiento de estados internos.

### 6. Timer  
**Archivo:** `Timer.c`  
- **Intención:** Ofrecer un mecanismo sencillo de conteo regresivo basado en el tick de sistema.  
- **Funcionalidad principal:**  
  - Inicio de temporizador grabando `HAL_GetTick()`.  
  - Disminución automática de un contador cada segundo.  
  - Señalización de fin de cuenta cuando llega a cero.

---
