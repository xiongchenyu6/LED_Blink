#include "rgb_breathing.h"
#include <math.h>

extern TIM_HandleTypeDef htim2;

/**
 * @brief Initialize RGB LED PWM
 */
void RGB_Init(void)
{
    // Start PWM on all three channels
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // Red   - PA0
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Green - PA1
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Blue  - PA2
    
    // Initialize with LED off
    RGB_SetColor(0, 0, 0);
}

/**
 * @brief Set RGB LED color with PWM values (0-999)
 * @param red: Red intensity (0-999)
 * @param green: Green intensity (0-999)
 * @param blue: Blue intensity (0-999)
 */
void RGB_SetColor(uint16_t red, uint16_t green, uint16_t blue)
{
    // Ensure values don't exceed timer period
    if (red > 999) red = 999;
    if (green > 999) green = 999;
    if (blue > 999) blue = 999;
    
    // Set PWM duty cycles
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, red);   // PA0 - Red
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, green); // PA1 - Green
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, blue);  // PA2 - Blue
}

/**
 * @brief Initialize breathing effect parameters
 * @param breathing: Pointer to breathing structure
 */
void RGB_BreathingInit(RGB_Breathing_t *breathing)
{
    breathing->step_count = 120;      // More steps for ultra-smooth breathing
    breathing->current_step = 0;
    breathing->delay_ms = 20;         // 20ms delay = smoother, less conflicts
    breathing->direction = 0;         // Start with fade in
    breathing->color_phase = 0;       // Start with red
}

/**
 * @brief Get rainbow color for given phase and intensity
 * @param phase: Color phase (0-5)
 * @param intensity: Brightness (0-999)
 * @return RGB color structure
 */
RGB_Color_t RGB_GetRainbowColor(uint8_t phase, uint16_t intensity)
{
    RGB_Color_t color = {0, 0, 0};
    
    switch(phase) {
        case 0: // Pure Red
            color.red = intensity;
            color.green = 0;
            color.blue = 0;
            break;
            
        case 1: // Red to Orange
            color.red = intensity;
            color.green = intensity / 2;
            color.blue = 0;
            break;
            
        case 2: // Orange to Yellow
            color.red = intensity;
            color.green = intensity;
            color.blue = 0;
            break;
            
        case 3: // Yellow to Green
            color.red = intensity / 2;
            color.green = intensity;
            color.blue = 0;
            break;
            
        case 4: // Pure Green
            color.red = 0;
            color.green = intensity;
            color.blue = 0;
            break;
            
        case 5: // Green to Cyan
            color.red = 0;
            color.green = intensity;
            color.blue = intensity / 2;
            break;
            
        case 6: // Pure Cyan
            color.red = 0;
            color.green = intensity;
            color.blue = intensity;
            break;
            
        case 7: // Cyan to Blue
            color.red = 0;
            color.green = intensity / 2;
            color.blue = intensity;
            break;
            
        case 8: // Pure Blue
            color.red = 0;
            color.green = 0;
            color.blue = intensity;
            break;
            
        case 9: // Blue to Purple
            color.red = intensity / 2;
            color.green = 0;
            color.blue = intensity;
            break;
            
        case 10: // Purple to Magenta
            color.red = intensity;
            color.green = 0;
            color.blue = intensity;
            break;
            
        case 11: // Magenta to Pink
            color.red = intensity;
            color.green = 0;
            color.blue = intensity / 2;
            break;
            
        default:
            color.red = intensity;
            color.green = 0;
            color.blue = 0;
            break;
    }
    
    return color;
}

/**
 * @brief Update breathing effect (call this regularly in main loop)
 * @param breathing: Pointer to breathing structure
 */
void RGB_BreathingUpdate(RGB_Breathing_t *breathing)
{
    static uint32_t last_update = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Check if it's time to update
    if (current_time - last_update < breathing->delay_ms) {
        return;
    }
    last_update = current_time;
    
    // Calculate breathing intensity using enhanced sine wave for smooth effect
    float progress = (float)breathing->current_step / breathing->step_count;
    uint16_t max_intensity = 950; // Higher max brightness for more vibrant colors
    
    // Use enhanced sine wave with subtle harmonics for more natural breathing
    float sine_main = sinf(progress * 3.14159f); // Main breathing wave
    float sine_harmonic = sinf(progress * 3.14159f * 2.0f) * 0.1f; // Subtle harmonic
    float combined_sine = sine_main + sine_harmonic;
    if (combined_sine > 1.0f) combined_sine = 1.0f; // Clamp to prevent overflow
    
    uint16_t intensity = (uint16_t)(combined_sine * max_intensity);
    
    // Get current rainbow color
    RGB_Color_t color = RGB_GetRainbowColor(breathing->color_phase, intensity);
    
    // Apply the color
    RGB_SetColor(color.red, color.green, color.blue);
    
    // Update step
    if (breathing->direction == 0) {
        // Fade in
        breathing->current_step++;
        if (breathing->current_step >= breathing->step_count) {
            breathing->direction = 1; // Switch to fade out
        }
    } else {
        // Fade out
        breathing->current_step--;
        if (breathing->current_step == 0) {
            breathing->direction = 0; // Switch to fade in
            // Move to next color phase
            breathing->color_phase++;
            if (breathing->color_phase > 11) { // Now 12 color phases (0-11)
                breathing->color_phase = 0;
            }
        }
    }
}