/*
 * This header file contains API for work with ADC channels:
 * temperature and battery voltage measurement.
 */

#include "mbed.h"

#define ADC_12BIT_RANGE 0x0FFF
#define ADC_16BIT_RANGE 0xFFFF
#define TEMPERATURE_MULTIPLIER_COEFFICIENT ((ADC_12BIT_RANGE * 30) / 14)
#define ADC_MAX_VOLTAGE 3.0f
#define BATTERY_DIVIDER_COEFFICIENT 2.135f
#define BATTERY_CONVERSION_COEFFICIENT (ADC_MAX_VOLTAGE * BATTERY_DIVIDER_COEFFICIENT)
#define WINC_CURRENT_DIVIDER_COEFFICIENT 2.122f
#define WINC_CURRENT_CONVERSION_COEFFICIENT (ADC_MAX_VOLTAGE * WINC_CURRENT_DIVIDER_COEFFICIENT)

/* TODO: Filter coefficients have to be recalculated for smartvalve
 *
 * =============================================================================
 *  Temperature (in degrees C) vs Normalized ADC count
 *  ------------------------------------------------
 *  We fit a 5th order polynomial to the Temp-versus-adc curve
 *  across the temperature range -55C to +70C.
 *
 *  The data is generated as follows:
 *       ADC count = [ 3.3v * R(T) / (28k + R(T) )  ]  * 4096 / 3.3v
 *       normalized ADC count =  (ADC count) / 4096.
 *                            = R(T) / (28k + R(T) )
 *
 *  The Thermistor B57221V2103 resistance curve R(T) is obtained from the
 *  R/T 8502 column in NTC_SMD_Standard_series_0402.pdf.
 *
 *  Coefficients are stored in Q13,0 format from highest order to lowest, i.e.
 *     T = tempCoefs[0] * adc^5 + tempCoefs[1] * adc^4 ...
 *
 *  (Q13,0 = integers that fit in 13 bits with no fractional bits.)
 *
 * =============================================================================
 */
static const int32_t tempCoefs[] = {-1601L, 4197L, -4298L, 2162L, -622L, 99L};

int temperature_conversion(int x) {
    /*  Since our polynomial fit is to adc normalized by the max 4096 value,
     *  we can treat the incoming x (ADC value) as a Q12,12 fixed-point number
     *  in the range [0,1).
     *  To fit all intermediate calculations into a 32-bit int,
     *  we need x in Q11,11 format.
     *  Add extra 1 shift due to normalize multiplier coefficient.
     */
    x = ((x * TEMPERATURE_MULTIPLIER_COEFFICIENT) / ADC_16BIT_RANGE) >> 2;

    // Set the other shifts we will need during calculation.
    const uint8_t qInput = 11;
    const uint8_t qtemp = 6;
    const uint8_t qCalc = (qInput + qtemp);

    // We'll accumulate results in temperature as a Q19,6.
    int32_t temperature = tempCoefs[0] << qtemp;  // Q13,0 -> Q19,6

    // To retain max precision during all the multiply-adds, we do them
    // conveniently in full Q30,17 precision and then convert back to Q19,6
    // for the next iteration.
    for (uint8_t i = 1; i <= 5; i++) {
        temperature *= x;                      // Q19,6  *  Q11,11 = Q30,17 Safe from overflow.
        temperature += tempCoefs[i] << qCalc;  // Q30,17 +  Q30,17          Full precision add.
        temperature >>= qInput;                // Q30,17 -> Q19,6
    }
    temperature >>= qtemp;  // Q19,6 -> Q13,0    i.e. integer output.
    return temperature;
}

/*
 * Function for conversion celcius degrees to fahrenheit.
 *  @param cel - int C
 *  @returns   - int F
 */
int celsius_to_fahrenheit(const int cel) {
    return (cel * 9) / 5 + 32;
}

/*
 * Function for conversion ADC value to battery voltage.
 *  @param x - int value from ADC, range 0 .. 65535
 *  @returns
 *    Value of battery voltage, float.
 */

float battery_voltage_conversion(const int x) {
    return ((float)x / ADC_16BIT_RANGE) * BATTERY_CONVERSION_COEFFICIENT;
}

/*
 * Function for conversion ADC value to WINC current consumption.
 *  @param x - int value from ADC, range 0 .. 65535
 *  @returns
 *    Value of WINC current consumption, float.
 */

float winc_current_conversion(const int x) {
    return ADC_MAX_VOLTAGE - (((float)x / ADC_16BIT_RANGE) * WINC_CURRENT_CONVERSION_COEFFICIENT);
}
