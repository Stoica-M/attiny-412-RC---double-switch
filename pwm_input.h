/*
 * pwm_input.h
 *
 * Created: 6/3/2025 12:41:00 PM
 * Author: Stoica
 */

#ifndef PWM_INPUT_H_
#define PWM_INPUT_H_

#include <stdint.h>
#include <stdbool.h>

// Initializes TCB0 to measure PWM pulse width on PA6
void pwm_input_init(void);

// Returns the measured pulse width in microseconds
uint16_t pwm_input_get_pulse_us(void);

// Checks whether the measured pulse width is within [min_us, max_us]
bool pwm_input_check_range(uint16_t min_us, uint16_t max_us);

#endif /* PWM_INPUT_H_ */
