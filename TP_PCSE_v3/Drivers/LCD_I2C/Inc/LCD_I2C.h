#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define SLAVE_ADDRESS_LCD 0x4E

#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_RS          0x01

extern I2C_HandleTypeDef hi2c1;

bool LCD_I2C_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_PrintString(const char *str);
void LCD_CreateChar(uint8_t location, uint8_t charmap[]);
void LCD_Send(uint8_t value, uint8_t mode);

#endif
