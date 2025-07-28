#ifndef CHINESE_FONT_16X16_H
#define CHINESE_FONT_16X16_H

#include <stdint.h>
#include "ssd1306_fonts.h"

// Chinese character font 16x16
extern const uint8_t ChineseFont16x16[];
extern FontDef Font_Chinese_16x16;

// Character mapping
#define CHAR_WO     0  // 我
#define CHAR_AI     1  // 爱
#define CHAR_GUO    2  // 郭
#define CHAR_ZHI    3  // 芷
#define CHAR_HUI    4  // 慧

// Function to draw Chinese character
void ssd1306_WriteChineseChar(uint8_t index, uint8_t color);
void ssd1306_WriteChineseText(uint8_t x, uint8_t y, uint8_t color);

#endif /* CHINESE_FONT_16X16_H */