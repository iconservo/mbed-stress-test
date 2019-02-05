/*
 * This file contains of hardware related functions and methods.
 */

#include "sv-hardware.h"

/*
 * Constructor for ButtonInput class.
 * It initializes input pins, local variables
 * and starts ticker for periodical polling of button pin.
 * Parameters:
 * @param butt   The name of pin, connected to button.
 * @param resol  The polling interval, us.
 */

ButtonInput::ButtonInput(PinName button, int resol) : button_interrupt(button, PullUp) {
    curr_state = button_interrupt.read();
    debounce_delay = resol;
    button_cbk = NULL;
    button_interrupt.fall(callback(this, &ButtonInput::debounceCallback));
    button_interrupt.rise(callback(this, &ButtonInput::debounceCallback));
}

/*
 * The callback function for button interrupt.
 * After interrupt debounce timer is run.
 */
void ButtonInput::debounceCallback(void) {
    int pin_tmp = button_interrupt.read();
    if (pin_tmp != curr_state) {
        mediate_state = pin_tmp;
        pin_checker.attach_us(callback(this, &ButtonInput::buttonStateChangeCallback), debounce_delay);
    }
}

/*
 * The callback function after debounce delay.
 * Button state is changed inside this function and callback is called.
 */
void ButtonInput::buttonStateChangeCallback(void) {
    int pin_tmp = button_interrupt.read();
    if (pin_tmp == mediate_state) {
        curr_state = pin_tmp;
        if (button_cbk) {
            button_cbk();
        }
    } else
        mediate_state = 3;
}
