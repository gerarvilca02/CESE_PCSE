#include "LCD_I2C.h"


static void I2C_Init(void){
	__HAL_RCC_I2C1_CLK_ENABLE();

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if (HAL_I2C_Init(&hi2c1) != HAL_OK){
		Error_Handler();
	}
}

static void LCD_ExpanderWrite(uint8_t data){
    // Transmisión I2C
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, &data, 1, HAL_MAX_DELAY);
}

static void LCD_Write4Bits(uint8_t nibble){
    LCD_ExpanderWrite(nibble | LCD_BACKLIGHT);
    LCD_ExpanderWrite(nibble | LCD_ENABLE | LCD_BACKLIGHT);
    LCD_ExpanderWrite((nibble & ~LCD_ENABLE) | LCD_BACKLIGHT);
}

void LCD_Send(uint8_t value, uint8_t mode){
    uint8_t highNib = (value & 0xF0) | mode;
    uint8_t lowNib  = ((value << 4) & 0xF0) | mode;
    LCD_Write4Bits(highNib);
    LCD_Write4Bits(lowNib);
}

bool LCD_I2C_Init(void){
	I2C_Init();
    HAL_Delay(50);
    LCD_ExpanderWrite(LCD_BACKLIGHT);
    // 0x30 x3
    LCD_Write4Bits(0x30);
    LCD_Write4Bits(0x30);
    LCD_Write4Bits(0x30);
    // Poner en modo 4 bits
    LCD_Write4Bits(0x20);
    // Función: 2 líneas, 5×8
    LCD_Send(0x28, 0);
    // Display off, clear, entry mode, display on
    LCD_Send(0x08, 0);
    LCD_Send(0x01, 0);
    LCD_Send(0x06, 0);
    LCD_Send(0x0C, 0);
    return true;
}
