# attiny-412-RC---double-switch
ATtiny412 Dual RC Switch with Dimming Control
This project implements a dual-channel RC switch on the ATtiny412 microcontroller, designed to respond to standard PWM signals from a radio receiver (RC). Each channel can toggle an LED or output pin based on received PWM pulse widths and additionally allows brightness control (dimming) if the button is held down for a longer period.

Features
✅ Dual channel RC-controlled switching

✅ PWM signal capture using TCB0 (Timer/Counter Type B)

✅ PWM output generation using TCA0 (Timer/Counter Type A)

✅ Dimming function triggered by long press

✅ Modes: simple on/off or PWM dimming

✅ No external components required besides the RC receiver and LEDs

How It Works
The firmware reads PWM signals (~1000–2000 µs) sent by an RC receiver and interprets them as button presses:

Short Press (>1850 µs): Toggles LED or output state.

Neutral Position (~1500 µs): Resets state and timers.

Long Press (>1850 µs for >3 seconds): Gradually dims the LED (reduces duty cycle).

Low Signal (<1150 µs): Toggles a second channel.

Each LED has independent logic, dimming thresholds, and state memory. This is useful for controlling lighting or other actuators via a standard RC remote.
