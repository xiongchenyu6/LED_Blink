#include "timer_control.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "chinese_font_16x16.h"
#include <stdio.h>

// Static variables for encoder tracking
static int32_t last_encoder_value = 0;
static bool last_button_state = false;
static uint32_t button_debounce_time = 0;

/**
 * @brief Initialize timer control
 * @param timer: Pointer to timer structure
 */
void Timer_Init(TimerControl_t *timer)
{
    timer->set_time = TIMER_DEFAULT;
    timer->remaining_time = TIMER_DEFAULT;
    timer->state = TIMER_STOPPED;
    timer->last_update = HAL_GetTick();
    timer->display_update = true;
    
    // Initialize encoder value
    last_encoder_value = 0;
}

/**
 * @brief Read encoder value using GPIO polling - simplified approach
 * @return Encoder position change
 */
int32_t Timer_ReadEncoderChange(void)
{
    static uint8_t last_a = 1, last_b = 1;
    static uint32_t last_change_time = 0;
    
    uint32_t current_time = HAL_GetTick();
    
    // Read current GPIO states (inverted because of pullup)
    uint8_t a = !HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin);  // PB0
    uint8_t b = !HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin);  // PB1
    
    // Simple edge detection with debouncing
    if (current_time - last_change_time < 20) {
        return 0; // Debounce
    }
    
    // Detect edge on A
    if (a != last_a) {
        last_change_time = current_time;
        last_a = a;
        
        if (a) { // Rising edge on A
            // Check B state to determine direction (reversed logic)
            if (b) {
                return -1; // Counter-clockwise (was clockwise)
            } else {
                return 1;  // Clockwise (was counter-clockwise)
            }
        }
    }
    
    // Update B state
    last_b = b;
    
    return 0;
}

/**
 * @brief Read button state with debouncing
 * @return True if button is pressed (falling edge)
 */
bool Timer_ReadButton(void)
{
    uint32_t current_time = HAL_GetTick();
    bool current_state = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin); // PB10 - Active low
    
    // Shorter debounce for better responsiveness
    if (current_time - button_debounce_time < 50) {
        return false;
    }
    
    // Detect falling edge (button press)
    if (current_state && !last_button_state) {
        last_button_state = current_state;
        button_debounce_time = current_time;
        return true;
    }
    
    // Update state after debounce period
    if (current_time - button_debounce_time >= 50) {
        last_button_state = current_state;
    }
    
    return false;
}

/**
 * @brief Handle encoder input to adjust timer value
 * @param timer: Pointer to timer structure
 */
void Timer_HandleEncoder(TimerControl_t *timer)
{
    // Only adjust timer when stopped
    if (timer->state != TIMER_STOPPED) {
        return;
    }
    
    int32_t encoder_change = Timer_ReadEncoderChange();
    
    if (encoder_change != 0) {
        // Remove LED debugging to prevent any GPIO conflicts
        
        // Adjust timer value
        int32_t new_time = (int32_t)timer->set_time + (encoder_change * TIMER_STEP);
        
        // Clamp to valid range
        if (new_time < TIMER_MIN_VALUE) {
            new_time = TIMER_MIN_VALUE;
        } else if (new_time > TIMER_MAX_VALUE) {
            new_time = TIMER_MAX_VALUE;
        }
        
        timer->set_time = (uint32_t)new_time;
        timer->remaining_time = timer->set_time;
        timer->display_update = true;
    }
}

/**
 * @brief Handle button press to start/stop timer
 * @param timer: Pointer to timer structure
 */
void Timer_HandleButton(TimerControl_t *timer)
{
    if (Timer_ReadButton()) {
        switch (timer->state) {
            case TIMER_STOPPED:
                // Start timer
                timer->state = TIMER_RUNNING;
                timer->remaining_time = timer->set_time;
                timer->last_update = HAL_GetTick();
                timer->display_update = true;
                break;
                
            case TIMER_RUNNING:
                // Stop timer
                timer->state = TIMER_STOPPED;
                timer->remaining_time = timer->set_time;
                timer->display_update = true;
                break;
                
            case TIMER_FINISHED:
                // Reset timer
                timer->state = TIMER_STOPPED;
                timer->remaining_time = timer->set_time;
                timer->display_update = true;
                break;
        }
    }
}

/**
 * @brief Update timer countdown
 * @param timer: Pointer to timer structure
 */
void Timer_Update(TimerControl_t *timer)
{
    if (timer->state != TIMER_RUNNING) {
        return;
    }
    
    uint32_t current_time = HAL_GetTick();
    
    // Check if 1 second has passed
    if (current_time - timer->last_update >= 1000) {
        timer->last_update = current_time;
        
        if (timer->remaining_time > 0) {
            timer->remaining_time--;
            timer->display_update = true;
            
            // Check if timer finished
            if (timer->remaining_time == 0) {
                timer->state = TIMER_FINISHED;
            }
        }
    }
}

/**
 * @brief Display timer on OLED
 * @param timer: Pointer to timer structure
 */
void Timer_DisplayOnOLED(TimerControl_t *timer)
{
    static uint32_t last_update = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Update every 500ms to avoid crashes
    if (current_time - last_update < 500) {
        return;
    }
    last_update = current_time;
    
    // Simple display update
    __disable_irq();
    
    ssd1306_Fill(0);  // Clear screen
    
    // Show timer value
    ssd1306_SetCursor(10, 10);
    ssd1306_WriteString("Timer:", Font_5x7, 1);
    
    // Display time in MM:SS:MS format (minutes:seconds:milliseconds)
    uint32_t display_time = (timer->state == TIMER_STOPPED) ? timer->set_time : timer->remaining_time;
    uint32_t tick_time = HAL_GetTick();
    
    // Convert to minutes and seconds
    uint32_t minutes = display_time / 60;
    uint32_t seconds = display_time % 60;
    
    // Get milliseconds - only update when timer is running
    uint32_t milliseconds;
    if (timer->state == TIMER_RUNNING) {
        milliseconds = (tick_time / 10) % 100;  // Live milliseconds when running
    } else {
        milliseconds = 0;  // Show 00 when stopped or finished
    }
    
    // Display MM:SS:MS format using Font_5x7
    ssd1306_SetCursor(15, 25);
    char single_char[2];
    single_char[1] = '\0';
    
    // Minutes (2 digits)
    single_char[0] = '0' + (minutes / 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    single_char[0] = '0' + (minutes % 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    // Colon
    single_char[0] = ':';
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    // Seconds (2 digits)
    single_char[0] = '0' + (seconds / 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    single_char[0] = '0' + (seconds % 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    // Colon
    single_char[0] = ':';
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    // Milliseconds (2 digits, 00-99)
    single_char[0] = '0' + (milliseconds / 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    single_char[0] = '0' + (milliseconds % 10);
    ssd1306_WriteString(single_char, Font_5x7, 1);
    
    // Show state
    ssd1306_SetCursor(10, 45);
    if (timer->state == TIMER_STOPPED) {
        ssd1306_WriteString("STOP", Font_5x7, 1);
    } else if (timer->state == TIMER_RUNNING) {
        ssd1306_WriteString("RUN", Font_5x7, 1);
    } else {
        ssd1306_WriteString("DONE", Font_5x7, 1);
    }
    
    ssd1306_UpdateScreen();
    __enable_irq();
}