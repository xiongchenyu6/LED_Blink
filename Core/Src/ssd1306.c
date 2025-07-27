#include "ssd1306.h" // <<< Ensure this line is present
#include "stm32f1xx_hal_i2c.h"

// Screen buffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static struct
{
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
} SSD1306;

// Send command
void ssd1306_WriteCommand(uint8_t command)
{
    // HAL_I2C_Mem_Write prototype is now known via ssd1306.h -> stm32f1xx_hal.h
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, HAL_MAX_DELAY);
}

// Send data
void ssd1306_WriteData(uint8_t *data, uint16_t size)
{
    // For large data transfers, we might need to split into smaller chunks
    uint16_t chunk_size = 32; // I2C buffer limitation
    uint16_t offset = 0;
    
    while (size > 0)
    {
        uint16_t current_chunk = (size > chunk_size) ? chunk_size : size;
        HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, 1, &data[offset], current_chunk, HAL_MAX_DELAY);
        offset += current_chunk;
        size -= current_chunk;
    }
}

// Initialize SSD1306
uint8_t ssd1306_Init(void)
{
    // Wait just a bit for the screen to boot
    HAL_Delay(20);

    // Check if device is ready
    // HAL_I2C_IsDeviceReady prototype is now known via ssd1306.h -> stm32f1xx_hal.h
    if (HAL_I2C_IsDeviceReady(&hi2c1, SSD1306_I2C_ADDR, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return 0; // Not found
    }

    // Minimal init sequence that works with most displays
    ssd1306_WriteCommand(0xAE); // Display OFF
    
    // Fundamental commands
    ssd1306_WriteCommand(0xA8); ssd1306_WriteCommand(0x3F); // Multiplex ratio = 64
    ssd1306_WriteCommand(0xD3); ssd1306_WriteCommand(0x00); // Display offset = 0
    ssd1306_WriteCommand(0x40); // Start line = 0
    ssd1306_WriteCommand(0xA1); // Segment remap
    ssd1306_WriteCommand(0xC8); // COM scan direction
    
    // Timing & Driving
    ssd1306_WriteCommand(0xD5); ssd1306_WriteCommand(0x80); // Clock divide
    ssd1306_WriteCommand(0xD9); ssd1306_WriteCommand(0x22); // Precharge
    ssd1306_WriteCommand(0xDB); ssd1306_WriteCommand(0x30); // VCOMH level
    
    // Hardware config
    ssd1306_WriteCommand(0xDA); ssd1306_WriteCommand(0x12); // COM pins
    
    // Charge pump
    ssd1306_WriteCommand(0x8D); ssd1306_WriteCommand(0x14); // Enable charge pump
    
    // Display
    ssd1306_WriteCommand(0x81); ssd1306_WriteCommand(0x7F); // Contrast
    ssd1306_WriteCommand(0xA4); // Display RAM content
    ssd1306_WriteCommand(0xA6); // Normal (not inverted)
    
    // Turn on
    ssd1306_WriteCommand(0xAF); // Display ON
    
    HAL_Delay(10); // Wait briefly for display to stabilize

    // Clear screen
    ssd1306_Fill(0); // 0 for Black, 1 for White

    // Flush buffer to screen
    ssd1306_UpdateScreen();

    // Set default values
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
    SSD1306.Initialized = 1;

    return 1; // OK
}

// Fill buffer
void ssd1306_Fill(uint8_t color)
{
    memset(SSD1306_Buffer, (color == 0) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

// Update screen
void ssd1306_UpdateScreen(void)
{
    // Using page addressing mode
    for (uint8_t page = 0; page < 8; page++)
    {
        ssd1306_WriteCommand(0xB0 + page); // Set page address (0xB0 to 0xB7)
        ssd1306_WriteCommand(0x00);         // Set lower column address
        ssd1306_WriteCommand(0x10);         // Set higher column address
        
        // Write one page of data (128 bytes)
        ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
    }
}

// Draw pixel
void ssd1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return; // Out of bounds
    }

    if (color == 1)
    { // White
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    { // Black
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

// Write character
char ssd1306_WriteChar(char ch, FontDef Font, uint8_t color)
{
    uint32_t i, b, j;

    // Check if it's the simple 5x7 font (stored as bytes)
    if (Font.FontWidth == 5 && Font.FontHeight == 7) {
        // Check remaining space
        if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
            SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
        {
            return 0; // Not enough space
        }
        
        // Direct byte access for 5x7 font
        const uint8_t *charData = (const uint8_t*)Font.data + (ch - 32) * 7;
        
        for (i = 0; i < Font.FontWidth; i++)
        {
            uint8_t column = charData[i];
            for (j = 0; j < Font.FontHeight; j++)
            {
                if (column & (1 << j))
                {
                    ssd1306_DrawPixel(SSD1306.CurrentX + i, SSD1306.CurrentY + j, color);
                }
                else
                {
                    ssd1306_DrawPixel(SSD1306.CurrentX + i, SSD1306.CurrentY + j, !color);
                }
            }
        }
        
        // Update cursor position - move right for next character
        SSD1306.CurrentX += Font.FontWidth + 1;
        return ch;
    }
    
    // Original code for other fonts (vertical rendering)
    // Check remaining space on current line
    if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontHeight) ||
        SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontWidth))
    {
        return 0; // Not enough space
    }

    // Rotate 90 degrees counterclockwise for vertical text
    for (i = 0; i < Font.FontHeight; i++)
    {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++)
        {
            if ((b << j) & 0x8000)
            {
                // Rotate: x becomes y, y becomes width-x
                ssd1306_DrawPixel(SSD1306.CurrentX + i, SSD1306.CurrentY + (Font.FontWidth - 1 - j), color);
            }
            else
            {
                ssd1306_DrawPixel(SSD1306.CurrentX + i, SSD1306.CurrentY + (Font.FontWidth - 1 - j), !color);
            }
        }
    }

    // Update cursor position for vertical text
    SSD1306.CurrentY += Font.FontWidth + 1; // Move down for next character
    return ch; // OK
}

// Write string
void ssd1306_WriteString(const char *str, FontDef Font, uint8_t color)
{
    while (*str)
    {
        if (ssd1306_WriteChar(*str, Font, color) != *str)
        {
            return; // Error
        }
        str++;
    }
}

// Set cursor position
void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}
