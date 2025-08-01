#include "chinese_font_16x16.h"
#include "ssd1306.h"

// External cursor position functions
extern void ssd1306_SetCursor(uint8_t x, uint8_t y);

// Chinese characters 16x16 font data
// Each character is 16x16 pixels = 32 bytes (2 bytes per row)
const uint8_t ChineseFont16x16[] = {
    // 我 (wo) - "I/me" - GB2312: CED2
    0x04, 0x80,  // .....1.. 1.......
    0x0E, 0xA0,  // ....111. 1.1.....
    0x78, 0x90,  // .1111... 1..1....
    0x08, 0x90,  // ....1... 1..1....
    0x08, 0x84,  // ....1... 1....1..
    0xFF, 0xFE,  // 11111111 1111111.
    0x08, 0x80,  // ....1... 1.......
    0x08, 0x90,  // ....1... 1..1....
    0x0A, 0x90,  // ....1.1. 1..1....
    0x0C, 0x60,  // ....11.. .11.....
    0x18, 0x40,  // ...11... .1......
    0x68, 0xA0,  // .11.1... 1.1.....
    0x09, 0x20,  // ....1..1 ..1.....
    0x0A, 0x14,  // ....1.1. ...1.1..
    0x28, 0x14,  // ..1.1... ...1.1..
    0x10, 0x0C,  // ...1.... ....11..
    
    // 爱 (ai) - "love" - GB2312: B0AE
    0x00, 0x78,  // ........ .1111...
    0x3F, 0x80,  // ..111111 1.......
    0x11, 0x10,  // ...1...1 ...1....
    0x09, 0x20,  // ....1..1 ..1.....
    0x7F, 0xFE,  // .1111111 1111111.
    0x42, 0x02,  // .1....1. ......1.
    0x82, 0x04,  // 1.....1. .....1..
    0x7F, 0xF8,  // .1111111 11111...
    0x04, 0x00,  // .....1.. ........
    0x07, 0xF0,  // .....111 1111....
    0x0A, 0x20,  // ....1.1. ..1.....
    0x09, 0x40,  // ....1..1 .1......
    0x10, 0x80,  // ...1.... 1.......
    0x11, 0x60,  // ...1...1 .11.....
    0x22, 0x1C,  // ..1...1. ...111..
    0x0C, 0x08,  // ....11.. ....1...
    
    // 郭 (guo) - surname Guo - GB2312: B9F9
    0x08, 0x00,  // ....1... ........
    0x04, 0x7C,  // .....1.. .11111..
    0xFF, 0xC4,  // 11111111 11...1..
    0x00, 0x44,  // ........ .1...1..
    0x3F, 0x48,  // ..111111 .1..1...
    0x21, 0x48,  // ..1....1 .1..1...
    0x21, 0x50,  // ..1....1 .1.1....
    0x3F, 0x48,  // ..111111 .1..1...
    0x00, 0x44,  // ........ .1...1..
    0x7F, 0x42,  // .1111111 .1....1.
    0x02, 0x42,  // ......1. .1....1.
    0x04, 0x62,  // .....1.. .11...1.
    0xFF, 0x54,  // 11111111 .1.1.1..
    0x04, 0x48,  // .....1.. .1..1...
    0x14, 0x40,  // ...1.1.. .1......
    0x08, 0x40,  // ....1... .1......
    
    // 芷 (zhi) - fragrant plant - GB2312: DCC6
    0x04, 0x40,  // .....1.. .1......
    0x04, 0x44,  // .....1.. .1...1..
    0xFF, 0xFE,  // 11111111 1111111.
    0x04, 0x40,  // .....1.. .1......
    0x05, 0x40,  // .....1.1 .1......
    0x01, 0x00,  // .......1 ........
    0x09, 0x00,  // ....1..1 ........
    0x09, 0x10,  // ....1..1 ...1....
    0x09, 0xF8,  // ....1..1 11111...
    0x09, 0x00,  // ....1..1 ........
    0x09, 0x00,  // ....1..1 ........
    0x09, 0x00,  // ....1..1 ........
    0x09, 0x00,  // ....1..1 ........
    0x09, 0x04,  // ....1..1 .....1..
    0xFF, 0xFE,  // 11111111 1111111.
    0x00, 0x00,  // ........ ........
    
    // 慧 (hui) - wisdom - GB2312: BBDB
    0x08, 0x20,  // ....1... ..1.....
    0x7E, 0xFC,  // .111111. 111111..
    0x08, 0x20,  // ....1... ..1.....
    0x3E, 0xF8,  // ..11111. 11111...
    0x08, 0x20,  // ....1... ..1.....
    0x7E, 0xFC,  // .111111. 111111..
    0x08, 0x20,  // ....1... ..1.....
    0x3F, 0xF8,  // ..111111 11111...
    0x00, 0x08,  // ........ ....1...
    0xFF, 0xFE,  // 11111111 1111111.
    0x00, 0x08,  // ........ ....1...
    0x3F, 0xF8,  // ..111111 11111...
    0x09, 0x00,  // ....1..1 ........
    0x28, 0xA8,  // ..1.1... 1.1.1...
    0x48, 0x24,  // .1..1... ..1..1..
    0x07, 0xE0,  // .....111 111.....
};

FontDef Font_Chinese_16x16 = {
    16,  // Width
    16,  // Height  
    (const uint16_t*)ChineseFont16x16
};

// Function to write a Chinese character at specific position  
void ssd1306_WriteChineseChar(uint8_t index, uint8_t color) {
    // This function is now deprecated - use ssd1306_WriteChineseText instead
    // Kept for compatibility but does nothing
    (void)index;
    (void)color;
}

// Pre-computed bitmap for "我" character only - ultra safe approach
// This eliminates all loops and pixel operations
const uint8_t PrecomputedWo[] = {
    // Row 0-7 (first page)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Row 8-15 (second page) 
    0x80, 0xA0, 0x90, 0x90, 0x84, 0xFE, 0x80, 0x90,
    0x90, 0x60, 0x40, 0xA0, 0x20, 0x14, 0x14, 0x0C
};

// Render Chinese characters HORIZONTALLY like the 5x7 font
void ssd1306_WriteChineseText(uint8_t x, uint8_t y, uint8_t color) {
    // External buffer reference
    extern uint8_t SSD1306_Buffer[128 * 64 / 8];
    
    // Render "我爱郭芷慧" horizontally using direct buffer access
    // This matches the 5x7 font rendering approach
    uint8_t char_indices[5] = {CHAR_WO, CHAR_AI, CHAR_GUO, CHAR_ZHI, CHAR_HUI};
    
    for (uint8_t charIdx = 0; charIdx < 5; charIdx++) {
        uint8_t charX = x + (charIdx * 17); // 16 width + 1 spacing
        
        // Bounds check
        if (charX + 16 > 128 || y + 16 > 64) break;
        
        const uint8_t *charData = ChineseFont16x16 + (char_indices[charIdx] * 32);
        
        // Render character horizontally (like 5x7 font does)
        for (uint8_t row = 0; row < 16; row++) {
            uint16_t rowData = (charData[row * 2] << 8) | charData[row * 2 + 1];
            
            // Calculate the Y position for this row
            uint8_t pixelY = y + row;
            if (pixelY >= 64) break;
            
            // Calculate buffer offset for this row
            uint8_t page = pixelY / 8;
            uint8_t bitPos = pixelY % 8;
            
            // Write the entire row of pixels at once
            for (uint8_t col = 0; col < 16; col++) {
                uint8_t pixelX = charX + col;
                if (pixelX >= 128) break;
                
                uint16_t bufferIdx = (page * 128) + pixelX;
                
                if (rowData & (0x8000 >> col)) {
                    // Set pixel
                    if (color) {
                        SSD1306_Buffer[bufferIdx] |= (1 << bitPos);
                    }
                } else {
                    // Clear pixel
                    if (!color) {
                        SSD1306_Buffer[bufferIdx] &= ~(1 << bitPos);
                    }
                }
            }
        }
    }
}