/*
 * TCA.h
 *
 * Created: 07/04/2024 15:11:32
 * Author: Stoica
 */

#ifndef TCA_H_
#define TCA_H_

// Threshold for RC signal in microseconds used to trigger actions (like LED toggle or dimming)
#define RC_THRESHOLD_US     1800

// PWM period and initial duty cycle values
#define PERIOD_VALUE        0xAFFF   // PWM period (timer top value)
#define DUTY_CYCLE_VALUE    0x0001   // Initial duty cycle (~0% brightness)

// State flags for RC switches and LED outputs
uint8_t switch1_state, switch2_state;
uint8_t prev_switch1_state, prev_switch2_state;
uint8_t led1_pwm_state, led2_pwm_state;

// Last captured RC pulse width in microseconds
uint16_t rc_pulse_width;

/**
 * @brief Main application loop that processes the RC input pulse.
 * 
 * @param pulse - the width of the RC pulse in microseconds
 */
void app_loop(uint16_t pulse);

/**
 * @brief Initializes TCA0 in PWM mode.
 */
void TCA0_init(void);

#endif /* TCA_H_ */
