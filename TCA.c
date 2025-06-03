/*
 * TCA.c
 *
 * Created: 07/04/2024 15:11:15
 * Author: Stoica
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "TCA.h"

void TCA0_init(void)
{
    // Configure TCA0 for single-slope PWM mode, enable channels 1 and 2
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc |
                        TCA_SINGLE_CMP1EN_bm |
                        TCA_SINGLE_CMP2EN_bm;

    // Enable TCA0 and enable overflow interrupt
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;

    // Set initial PWM period
    TCA0.SINGLE.PER = PERIOD_VALUE;
}

// Variables for dimming timing
volatile uint16_t press_time_ch1 = 0, press_time_ch2 = 0;
volatile uint16_t dim_timer = 0;

uint8_t dim_active_ch1 = 0, dim_active_ch2 = 0;

// PWM control variables for both channels
uint16_t pwm_ch1 = 0;
uint16_t pwm_ch1_limit = PERIOD_VALUE;
uint16_t pwm_ch2 = 0;
uint16_t pwm_ch2_limit = PERIOD_VALUE;

#define DIM_THRESHOLD 800  // ~8 seconds if overflow is every 10ms

// Overflow interrupt: measure how long each "button" is pressed
ISR(TCA0_OVF_vect)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Clear overflow flag

    // Check RC pulse width and increment press timers if in active range
    if ((rc_pulse_width > 1850) && (rc_pulse_width < 2150)) {
        press_time_ch1++;
    }
    if ((rc_pulse_width > 900) && (rc_pulse_width < 1150)) {
        press_time_ch2++;
    }
}

void app_loop(uint16_t pulse)
{
    // Update global pulse width from RC
    rc_pulse_width = pulse;

    // Process RC switch states
    if (rc_pulse_width > 1850) {
        switch1_state = 1;
    }
    if (rc_pulse_width < 1150) {
        switch2_state = 1;
    }
    else if ((rc_pulse_width > 1350) && (rc_pulse_width < 1650)) {
        switch1_state = 0;
        switch2_state = 0;
        press_time_ch1 = 0;
        press_time_ch2 = 0;

        // Reset PWM if LED is off
        if (led1_pwm_state == 0) pwm_ch1 = 0;
        if (led2_pwm_state == 0) pwm_ch2 = 0;
    }

    // Channel 1: toggle LED state on rising edge
    if (switch1_state != prev_switch1_state && switch1_state == 1) {
        led1_pwm_state ^= 1;
    }

    if (led1_pwm_state == 1) {
        if ((switch1_state == 1) && (press_time_ch1 > DIM_THRESHOLD)) {
            dim_active_ch1 = 1;
            TCA0.SINGLE.CMP1 = pwm_ch1_limit--;
            if (pwm_ch1_limit >= PERIOD_VALUE) pwm_ch1_limit = PERIOD_VALUE;
        } else {
            if (dim_active_ch1 == 0) {
                TCA0.SINGLE.CMP1 = pwm_ch1 += 4;
                if (pwm_ch1 >= pwm_ch1_limit) pwm_ch1 = pwm_ch1_limit;
            }
        }
    } else {
        TCA0.SINGLE.CMP1 = 0;
        dim_active_ch1 = 0;
    }

    prev_switch1_state = switch1_state;

    // Channel 2: toggle LED state on rising edge
    if (switch2_state != prev_switch2_state && switch2_state == 1) {
        led2_pwm_state ^= 1;
    }

    if (led2_pwm_state == 1) {
        if ((switch2_state == 1) && (press_time_ch2 > DIM_THRESHOLD)) {
            dim_active_ch2 = 1;
            TCA0.SINGLE.CMP2 = pwm_ch2_limit--;
            if (pwm_ch2_limit >= PERIOD_VALUE) pwm_ch2_limit = PERIOD_VALUE;
        } else {
            if (dim_active_ch2 == 0) {
                TCA0.SINGLE.CMP2 = pwm_ch2 += 4;
                if (pwm_ch2 >= pwm_ch2_limit) pwm_ch2 = pwm_ch2_limit;
            }
        }
    } else {
        TCA0.SINGLE.CMP2 = 0;
        dim_active_ch2 = 0;
    }

    prev_switch2_state = switch2_state;
}
