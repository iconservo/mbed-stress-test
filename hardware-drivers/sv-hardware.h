/*
 * This header file contains API for work with functionality,
 * related to SoC GPIO.
 */
#ifndef SV_HARDWARE_H
#define SV_HARDWARE_H

#include "mbed.h"

#if EMBIGGEN_IOT_PLATFORM
#define ON_OFF_BUTTON_PIN ON_OFF_BUTTON
#define WAKE_BUTTON_PIN WAKE_BUTTON
#else
#define ON_OFF_BUTTON_PIN BUTTON1
#define WAKE_BUTTON_PIN BUTTON2
#endif

#define BUTTON_RESOLUTION_US 20000

class ButtonInput {
   public:
    /** Create a ButtonInput connected to the specified pin
     *
     *  @param  pin DigitalIn pin to connect to
     *  @param  resol - time of pin polling, us
     */
    ButtonInput(PinName button, int resol);

    /** Returns current button state
     *
     *  @returns
     *    An integer representing the state of the input pin,
     *    0 for logical 0, 1 for logical 1
     */
    int getButtonState(void) { return curr_state; }

    /** Attach external callback function
     *
     */
    void attach(Callback<void()> cb) { button_cbk = cb; }

   private:
    /** Callback for ticker
     *
     */
    void debounceCallback(void);
    void buttonStateChangeCallback(void);

    InterruptIn button_interrupt;
    Timeout pin_checker;
    int curr_state;
    int mediate_state;
    int debounce_delay;
    Callback<void()> button_cbk;
};

#endif /* SV_HARDWARE_H */
