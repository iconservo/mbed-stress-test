//------------------------------------------------------------------------------
#ifndef _RPM_SENSOR_H_
#define _RPM_SENSOR_H_
//------------------------------------------------------------------------------
#include "mbed.h"
#include "PinNames.h"
//------------------------------------------------------------------------------

#ifdef TARGET_NRF52_DK
#define RPM_SENSOR p5
#endif

class RpmSensor {
   private:
    uint32_t count;
    int rate_count;
    float rate;
    int rate_last_click_t, rate_first_click_t;  // local timestamps in ms
    bool rpm_irq_status;
    InterruptIn rpm_int_in_;
    LowPowerTimer timer_;
    LowPowerTicker rate_update_ticker_;

    // The callback function for RPM input pin change interrupt
    void clickCallback(void) {
        count++;
        rate_last_click_t = timer_.read_ms();
        if (rate_count == -1) {
            rate_first_click_t = rate_last_click_t;
        }
        rate_count++;
    }

    // The callback function for RPM rate update ticker
    void rateUpdateCallback(void) {
        if (rate_first_click_t != rate_last_click_t) {
            rate = (float)rate_count * 1e3 / (rate_last_click_t - rate_first_click_t);  // clicks/s
        } else {
            rate = 0.0;
        }
        rate_first_click_t = rate_last_click_t;
        rate_count = -1;  // Trigger rate_first_click_t update in clickCallback
        // Reset timestamps timer to avoid its overflow
        timer_.reset();
    }

   public:
    // Creates RPM Sensor instance
    //   rpm_pin - GPIO RPM sensor connected to
    //   rate_update_interval - RPM rate update interval in seconds
    RpmSensor(PinName rpm_pin = RPM_SENSOR, float rate_update_interval = 2.0)
        : count(0),
          rate_count(-1),  // Trigger rate_first_click_t update in clickCallback
          rate(0.0),
          rate_last_click_t(0),
          rate_first_click_t(0),
          rpm_int_in_(rpm_pin, PullUp) {
        rpm_int_in_.fall(callback(this, &RpmSensor::clickCallback));
        rate_update_ticker_.attach(callback(this, &RpmSensor::rateUpdateCallback), rate_update_interval);
        rpm_irq_status = 1;
        timer_.start();
    }

    ~RpmSensor() {}

    void enable() {
        rpm_int_in_.enable_irq();
        rpm_irq_status = true;
    }

    void disable() {
        rpm_int_in_.disable_irq();
        rpm_irq_status = false;
    }

    void reset() {
        count = 0;
        rate_first_click_t = rate_last_click_t;
        rate_count = -1;  // Trigger rate_first_click_t update in clickCallback
    }

    uint32_t getCount() { return count; }

    float getRate() { return rate; }

    int getRpmIrqStatus() { return rpm_irq_status; }
};

//------------------------------------------------------------------------------
#endif  // _RPM_SENSOR_H_
//------------------------------------------------------------------------------
