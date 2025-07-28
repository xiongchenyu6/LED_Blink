#ifndef TIMER_CONTROL_H
#define TIMER_CONTROL_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

// Timer states
typedef enum {
    TIMER_STOPPED,
    TIMER_RUNNING,
    TIMER_FINISHED
} TimerState_t;

// Timer control structure
typedef struct {
    uint32_t set_time;       // Timer value in seconds
    uint32_t remaining_time; // Remaining time in seconds
    TimerState_t state;      // Current timer state
    uint32_t last_update;    // Last update timestamp
    bool display_update;     // Flag to update display
} TimerControl_t;

// Function prototypes
void Timer_Init(TimerControl_t *timer);
void Timer_HandleEncoder(TimerControl_t *timer);
void Timer_HandleButton(TimerControl_t *timer);
void Timer_Update(TimerControl_t *timer);
void Timer_DisplayOnOLED(TimerControl_t *timer);
uint32_t Timer_ReadEncoder(void);
bool Timer_ReadButton(void);

// Timer settings
#define TIMER_MIN_VALUE     1    // Minimum 1 second
#define TIMER_MAX_VALUE     3600 // Maximum 1 hour
#define TIMER_STEP          5    // 5 second increments
#define TIMER_DEFAULT       60   // Default 60 seconds

// GPIO definitions
#define ENCODER_A_GPIO_Port GPIOB
#define ENCODER_A_Pin       GPIO_PIN_0
#define ENCODER_B_GPIO_Port GPIOB  
#define ENCODER_B_Pin       GPIO_PIN_1
#define BUTTON_GPIO_Port    GPIOB
#define BUTTON_Pin          GPIO_PIN_10

#endif /* TIMER_CONTROL_H */