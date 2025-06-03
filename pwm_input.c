/*
 * pwm_input.c
 *
 * Created: 6/3/2025 12:41:37 PM
 * Author: Stoica
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm_input.h"

static volatile uint16_t pwm_pulse_ticks = 0;

void pwm_input_init(void)
{
    // configure PA6 as input
    PORTA.DIRCLR = PIN6_bm;

    // setup Event System: PA6 ? asynchronous channel 0
    EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_PORTA_PIN6_gc;
    EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH0_gc;

    // configure TCB0 in Pulse Width Capture mode
    TCB0.CTRLB = TCB_CNTMODE_PW_gc;      // Pulse Width measurement mode
    TCB0.EVCTRL = TCB_CAPTEI_bm;         // Capture event input enabled
    TCB0.INTCTRL = TCB_CAPT_bm;          // Enable interrupt on capture
    TCB0.CTRLA = TCB_ENABLE_bm;          // Enable the timer
}

ISR(TCB0_INT_vect)
{
    // read captured pulse width in timer ticks
    pwm_pulse_ticks = TCB0.CCMP;

    // clear capture interrupt flag
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

bool pwm_input_check_range(uint16_t min_us, uint16_t max_us)
{
    // convert pulse ticks to microseconds, assuming F_CPU = 20 MHz
    uint64_t pulse_us = ((uint64_t)pwm_pulse_ticks * 1000000UL) / 20000000UL;
    return (pulse_us >= min_us && pulse_us <= max_us);
}

uint16_t pwm_input_get_pulse_us(void)
{
    // convert and return pulse width in microseconds
    return ((uint64_t)pwm_pulse_ticks * 1000000UL) / 20000000UL;
}
