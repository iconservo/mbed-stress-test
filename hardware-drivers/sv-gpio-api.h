/*
 * This header file contains API for work with functionality,
 * related to SoC GPIO.
 */
#ifndef SV_GPIO_API_H
#define SV_GPIO_API_H

#include "sv-hardware.h"

#ifndef SV_BUTTON_TEST
#define SV_BUTTON_TEST 0
#endif

class SvButton {
   public:
    /** Create a SvButton connected to the specified pin
     *
     *  @param pin - pin to connect to
     */
    SvButton(PinName pin) : sv_button_(pin, BUTTON_RESOLUTION_US) {}

    /** Returns button state
     *
     *  @returns
     *    An boolean representing the state of the button,
     *    1 if button is pressed, 0 otherwise
     */
    const int isPressed() { return (!sv_button_.getButtonState()); }

    /** Attach external callback function
     *
     */
    void attach(Callback<void()> cb) { sv_button_.attach(cb); }

   private:
    ButtonInput sv_button_;
};

#endif /* SV_GPIO_API_H */
