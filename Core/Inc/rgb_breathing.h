#ifndef RGB_BREATHING_H
#define RGB_BREATHING_H

#include "stm32f1xx_hal.h"
#include "tim.h"

// RGB color structure
typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} RGB_Color_t;

// Breathing effect parameters
typedef struct {
    uint32_t step_count;
    uint32_t current_step;
    uint32_t delay_ms;
    uint8_t direction;  // 0 = fade in, 1 = fade out
    uint8_t color_phase; // 0-5 for color cycling
} RGB_Breathing_t;

// Function prototypes
void RGB_Init(void);
void RGB_SetColor(uint16_t red, uint16_t green, uint16_t blue);
void RGB_BreathingInit(RGB_Breathing_t *breathing);
void RGB_BreathingUpdate(RGB_Breathing_t *breathing);
RGB_Color_t RGB_GetRainbowColor(uint8_t phase, uint16_t intensity);

// Predefined colors
#define RGB_RED     {1000, 0, 0}
#define RGB_GREEN   {0, 1000, 0}
#define RGB_BLUE    {0, 0, 1000}
#define RGB_YELLOW  {1000, 1000, 0}
#define RGB_CYAN    {0, 1000, 1000}
#define RGB_MAGENTA {1000, 0, 1000}
#define RGB_WHITE   {1000, 1000, 1000}
#define RGB_OFF     {0, 0, 0}

#endif /* RGB_BREATHING_H */