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
    uint8_t a = !HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin);
    uint8_t b = !HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin);
    
    // Simple edge detection with debouncing
    if (current_time - last_change_time < 20) {
        return 0; // Debounce
    }
    
    // Detect edge on A
    if (a != last_a) {
        last_change_time = current_time;
        last_a = a;
        
        if (a) { // Rising edge on A
            // Check B state to determine direction
            if (b) {
                return 1;  // Clockwise
            } else {
                return -1; // Counter-clockwise
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
    bool current_state = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin); // Active low
    
    // Better debounce timing - longer delay
    if (current_time - button_debounce_time < 200) {
        return false;
    }
    
    // Detect falling edge (button press)
    if (current_state && !last_button_state) {
        last_button_state = current_state;
        button_debounce_time = current_time;
        return true;
    }
    
    // Update state after debounce period
    if (current_time - button_debounce_time > 200) {
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
    static bool show_timer_interface = false;
    static uint32_t last_encoder_interaction = 0;
    static uint32_t last_button_interaction = 0;
    static uint32_t last_display_time = 0;
    static uint8_t last_display_state = 255;
    
    // Check if we should show timer interface
    uint32_t current_time = HAL_GetTick();
    bool timer_active = (timer->state == TIMER_RUNNING || timer->state == TIMER_FINISHED);
    bool recent_interaction = (current_time - last_encoder_interaction < 3000) || 
                             (current_time - last_button_interaction < 3000);
    
    // Update interaction timestamps
    if (timer->display_update) {
        if (timer->state == TIMER_RUNNING || timer->state == TIMER_FINISHED) {
            last_button_interaction = current_time;
        }
        if (timer->state == TIMER_STOPPED) {
            last_encoder_interaction = current_time;
        }
    }
    
    // Decide what to show
    bool should_show_timer = timer_active || recent_interaction;
    
    // Get current display time for comparison
    uint32_t display_time = (timer->state == TIMER_STOPPED) ? timer->set_time : timer->remaining_time;
    
    // Only update if something actually changed
    bool needs_update = (should_show_timer != show_timer_interface) ||
                       (display_time != last_display_time) ||
                       (timer->state != last_display_state) ||
                       timer->display_update;
    
    if (needs_update) {
        show_timer_interface = should_show_timer;
        last_display_time = display_time;
        last_display_state = timer->state;
        timer->display_update = false;
        
        // Disable all interrupts for stability
        __disable_irq();
        
        // Clear screen completely
        ssd1306_Fill(0);
        
        if (show_timer_interface) {
            // Simple timer display - just the essentials
            
            // Timer title
            ssd1306_SetCursor(40, 5);
            ssd1306_WriteString("TIMER", Font_7x10, 1);
            
            // Format and display time
            uint32_t minutes = display_time / 60;
            uint32_t seconds = display_time % 60;
            
            // Use simple string formatting to avoid corruption
            char min_str[4];
            char sec_str[4];
            min_str[0] = '0' + (minutes / 10);
            min_str[1] = '0' + (minutes % 10);
            min_str[2] = '\0';
            sec_str[0] = '0' + (seconds / 10);
            sec_str[1] = '0' + (seconds % 10);
            sec_str[2] = '\0';
            
            // Display time pieces separately
            ssd1306_SetCursor(30, 25);
            ssd1306_WriteString(min_str, Font_11x18, 1);
            ssd1306_SetCursor(52, 25);
            ssd1306_WriteString(":", Font_11x18, 1);
            ssd1306_SetCursor(65, 25);
            ssd1306_WriteString(sec_str, Font_11x18, 1);
            
            // Simple status
            ssd1306_SetCursor(25, 50);
            if (timer->state == TIMER_STOPPED) {
                ssd1306_WriteString("Turn Encoder", Font_7x10, 1);
            } else if (timer->state == TIMER_RUNNING) {
                ssd1306_WriteString("RUNNING", Font_7x10, 1);
            } else {
                ssd1306_WriteString("TIME UP!", Font_7x10, 1);
            }
            
        } else {
            // Show Chinese text
            ssd1306_WriteChineseText(14, 24, 1);
        }
        
        // Update screen
        ssd1306_UpdateScreen();
        
        // Re-enable interrupts
        __enable_irq();
    }
}