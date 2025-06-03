/*
 * attiny_412_RC_dual_switch.c
 *
 * Created: 6/3/2025 4:43:17 PM
 * Author: Stoica
 */

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pwm_input.h"


#define  mode_pwm
//#define  mode_simplu

#ifdef mode_pwm
#include "TCA.h"
#endif

// global RC pulse variable
uint16_t rc_pulse_width = 0;

// switch state variables
uint8_t switch1_state = 0, switch2_state = 0;
uint8_t prev_switch1_state = 0, prev_switch2_state = 0;

// led state for simple mode
uint8_t led1_state = 0, led2_state = 0;



// prototypes
void switch_simple_mode(void);
void PORT_init(void);

int main(void)
{
    // disable prescaler (20 MHz clock)
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);

    PORT_init();

    #ifdef mode_pwm
    TCA0_init();
    #endif

    pwm_input_init();
    sei(); // enable global interrupts

    while (1) {

        if (pwm_input_check_range(800, 2200)) {

            #if defined(mode_simplu)
                switch_simple_mode();
                rc_pulse_width = pwm_input_get_pulse_us();

            #elif defined(mode_pwm)
                rc_pulse_width = pwm_input_get_pulse_us();
                app_loop(rc_pulse_width);

            #else
                #error "Trebuie definit fie mode_simplu, fie mode_pwm"
            #endif

        } else {
            #ifdef mode_pwm
                TCA0.SINGLE.CMP1 = 0;
                TCA0.SINGLE.CMP2 = 0;
            #endif

            PORTA.OUTCLR = PIN1_bm | PIN2_bm;
        }
    }
}

void switch_simple_mode(void)
{
    uint16_t pulse = pwm_input_get_pulse_us();

    if (pulse > 1850) {
        switch1_state = 1;
    }
    if (pulse < 1150) {
        switch2_state = 1;
    }
    else if ((pulse > 1350) && (pulse < 1650)) {
        switch1_state = 0;
        switch2_state = 0;
    }

    if (switch1_state != prev_switch1_state && switch1_state == 1) {
        PORTA.OUTTGL = PIN1_bm;
    }
    prev_switch1_state = switch1_state;

    if (switch2_state != prev_switch2_state && switch2_state == 1) {
        PORTA.OUTTGL = PIN2_bm;
    }
    prev_switch2_state = switch2_state;
}

void PORT_init(void)
{
    PORTA.DIRSET = PIN1_bm | PIN2_bm;
    PORTA.OUTCLR = PIN1_bm | PIN2_bm;
}
