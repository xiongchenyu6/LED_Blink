#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#include "stm32f1xx_hal.h" // Include the HAL definitions first
#include "stm32f1xx_hal_i2c.h"
#include "ssd1306_fonts.h"
#include <string.h>

// I2C handle defined in main.c (or i2c.c if using CubeMX)
// Ensure this handle is correctly defined and initialized elsewhere!
extern I2C_HandleTypeDef hi2c1;

// SSD1306 I2C address
// Note: Some modules use 0x3D instead of 0x3C
// The address is shifted left by 1 bit for HAL I2C functions
#define SSD1306_I2C_ADDR (0x3C << 1) // Use 0x3C or 0x3D based on your display
//#define SSD1306_I2C_ADDR (0x3D << 1) // Alternative address

// SSD1306 Screen size
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Colors
#define SSD1306_COLOR_BLACK 0x00
#define SSD1306_COLOR_WHITE 0x01

// Initialization function
uint8_t ssd1306_Init(void);

// Fill the screen buffer with a color
void ssd1306_Fill(uint8_t color);

// Write a command to the SSD1306 controller
void ssd1306_WriteCommand(uint8_t command);

// Write data to the SSD1306 controller
void ssd1306_WriteData(uint8_t *data, uint16_t size);

// Set cursor position
void ssd1306_SetCursor(uint8_t x, uint8_t y);

// Write a string to the display
void ssd1306_WriteString(const char *str, FontDef Font, uint8_t color);

// Update display (send buffer to display)
void ssd1306_UpdateScreen(void);

// Draw a pixel
void ssd1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);

#endif /* INC_SSD1306_H_ */
