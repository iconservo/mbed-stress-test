/*
 * This header file contains API for work with ADC channels:
 * temperature and battery voltage measurement.
 */

#ifndef HARDWARE_DRIVERS_SV_ANALOG_H_
#define HARDWARE_DRIVERS_SV_ANALOG_H_

#include "lowpwr.h"

#if !EMBIGGEN_IOT_PLATFORM
#define TEMP_SENSOR P0_3
#define VBATT_SENSOR P0_4
#define VBAT_SENSE_ENABLE P0_22
#define WINC_CURRENT_SENSOR P0_2
#define WINC_CURRENT_SENSOR_ENABLE P0_9
#endif

int temperature_conversion(int x);
int celsius_to_fahrenheit(const int cel);
float battery_voltage_conversion(int x);
float winc_current_conversion(int x);
void printf_all(const char* format, ...);

class SvTemperature {
   public:
    /** Create a SvTemperature connected to the specified pin
     *
     */
    SvTemperature() : temp_sensor_pin_(TEMP_SENSOR) {}
    /** Read ADC value for temperature, normalized to u_int16
     *
     *  @returns
     *    ADC value for temperature.
     */
    int read_raw() {
#if EMBIGGEN_IOT_PLATFORM
        if (!LowPwrState())
            printf_all("Low power source is off! \r\n");
#endif
        return temp_sensor_pin_.read_u16();
    }
    /** Read temperature in celsius degrees.
     *
     *  @returns
     *    Value of temperature.
     */
    int read() {
        int value = read_raw();
        return temperature_conversion(value);
    }

   private:
    AnalogIn temp_sensor_pin_;
};

class SvBatteryVoltage {
   public:
    /** Create a SvBatteryVoltage connected to the specified pin
     *
     */
    SvBatteryVoltage() : battery_voltage_pin_(VBATT_SENSOR), voltage_connect_pin_(VBAT_SENSE_ENABLE, 0) {}
    /** Read ADC value for battery voltage, normalized to u_int16
     *
     *  @returns
     *    ADC value for battery voltage.
     */
    int read_raw() {
        measurement_on();
        int value = battery_voltage_pin_.read_u16();
        measurement_off();
        return value;
    }
    /** Read battery voltage in volts.
     *
     *  @returns
     *    Value of battery voltage.
     */
    float read() { return battery_voltage_conversion(read_raw()); }
    void measurement_on(void) {
        voltage_connect_pin_.write(1);
        wait_ms(200);
    }
    void measurement_off(void) { voltage_connect_pin_.write(0); }

   private:
    AnalogIn battery_voltage_pin_;
    DigitalOut voltage_connect_pin_;
};
class SvWINCCurrent {
   public:
    /** Create a SvWINCCurrent connected to the specified pin
     *
     */
    SvWINCCurrent()
        : winc_current_pin_(WINC_CURRENT_SENSOR), winc_current_connect_pin_(WINC_CURRENT_SENSOR_ENABLE, 0) {}
    /** Read ADC value for WINC current consumption, normalized to u_int16
     *
     *  @returns
     *    ADC value for WINC current consumption.
     */
    int read_raw() {
        measurement_on();
        int value = winc_current_pin_.read_u16();
        measurement_off();
        return value;
    }
    /** Read WINC current consumption in ampers.
     *
     *  @returns
     *    Value of WINC current consumption.
     */
    float read() { return winc_current_conversion(read_raw()); }
    void measurement_on(void) {
        winc_current_connect_pin_.write(1);
        wait_ms(10);
    }
    void measurement_off(void) { winc_current_connect_pin_.write(0); }

   private:
    AnalogIn winc_current_pin_;
    DigitalOut winc_current_connect_pin_;
};

#endif /* HARDWARE_DRIVERS_SV_ANALOG_H_ */
