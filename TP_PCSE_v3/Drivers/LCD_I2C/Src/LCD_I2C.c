#include "LCD_I2C.h"


void LCD_Clear(void){
    LCD_Send(0x01, 0);
}

void LCD_SetCursor(uint8_t row, uint8_t col){
    static const uint8_t row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1;
    LCD_Send(0x80 | (col + row_offsets[row]), 0);
}

void LCD_PrintString(const char *str){
    while (*str){
        LCD_Send((uint8_t)*str++, LCD_RS);
    }
}

void LCD_CreateChar(uint8_t location, uint8_t charmap[]){
	LCD_Send(0x40 | (location << 3), 0);
	for (uint8_t i = 0; i < 8; i++){
		LCD_Send(charmap[i], LCD_RS);
	}
}
