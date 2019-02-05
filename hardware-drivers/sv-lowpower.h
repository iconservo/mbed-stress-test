/*
 * This header file contains API for work with low power source.
 *
 */

#ifndef HARDWARE_DRIVERS_SV_LOWPOWER_H_
#define HARDWARE_DRIVERS_SV_LOWPOWER_H_

#ifdef TARGET_NRF52_DK
#define LOWPWR_ENABLE P0_23
#endif

class SvLowPower {
   public:
    /** Create a SvButton connected to the specified pin
     *
     *  @param pin - pin to connect to
     */
    SvLowPower() : lowp_enable_pin_(LOWPWR_ENABLE, 0) {
        users_count_ = 0;
        on_ = 0;
    }
    /** Switches on low power source, wait stabilization time,
     *   increase counter of users. If low power source is already ON,
     *   returns TRUE immediately.
     *
     *  @returns
     *    True for now.
     */
    bool obtain() {
        if (!on_) {
            lowp_enable_pin_.write(1);
            wait_ms(1);
            on_ = 1;
        }
        users_count_++;
        return true;
    }
    /** Decrease the counter of users, switches off low power source
     * if counter equal to 0.
     *
     *  @returns
     *    True for now.
     */
    bool release() {
        users_count_--;
        if (users_count_ < 0) {
            printf("## ERROR, extra release of low power source \n");
            users_count_ = 0;
        }
        if (users_count_ == 0) {
            lowp_enable_pin_.write(0);
            on_ = 0;
        }
        return true;
    }

   private:
    DigitalOut lowp_enable_pin_;
    int users_count_;
    int on_;
};

#endif /* HARDWARE_DRIVERS_SV_LOWPOWER_H_ */
