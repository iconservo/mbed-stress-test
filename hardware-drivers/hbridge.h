/*
 * This header file contains API for work with Solenoid,
 *
 */

#ifndef __HBRIDGE_H__
#define __HBRIDGE_H__

#include "mbed.h"

#if EMBIGGEN_IOT_PLATFORM
#define HDRIVE_INA_PIN HDRIVE_INA
#define HDRIVE_INB_PIN HDRIVE_INB
#define HDRIVE_EN_PIN HDRIVE_EN
#define HDRIVE_BOOST_PIN HDRIVE_BOOST
#else
#define HDRIVE_INA_PIN p19
#define HDRIVE_INB_PIN p20
#define HDRIVE_EN_PIN p17
#define HDRIVE_BOOST_PIN p30
#endif

#define DEFAULT_BOOST_CHARGE_OPEN_MS 100
#define DEFAULT_BOOST_CHARGE_CLOSE_MS 50
#define DEFAULT_HBRIDGE_ENABLE_DELAY_MS 10
#define DEFAULT_HBRIDGE_ENABLE_ON_MS 200

class Solenoid {
   public:
    enum Action { Open = 0, Close };

   protected:
    DigitalOut solenoid_ina;
    DigitalOut solenoid_inb;
    DigitalOut solenoid_en;
    DigitalOut solenoid_boost;
    mbed::LowPowerTimeout solenoid_ticker;

    enum Event { Start = 0, En_bridge, Fire, Dis_bridge };
    enum State { Idle = 0, BoostLatched, HbridgeEnableDelay, Pulse };
    State state;

    Action action;

    Event event;

    /** Setup timer for callback solenoid state machine function.
     *
     */
    void sendEvent(float delay_ms = 0) {
        if (delay_ms > 0) {
            solenoid_ticker.attach(mbed::callback(this, &Solenoid::sequencer), delay_ms / 1000);
        } else {
            sequencer();
        }
    }
    void sequencer();

    bool valve_opened;

    float boost_charge_open_ms, boost_charge_close_ms, hbridge_enable_delay_ms, hbridge_enable_on_ms;

   public:
    /** Create a Solenoid object. Initialize connected pins, set default time delay for state machine.
     *
     */
    Solenoid(PinName ina, PinName inb, PinName en, PinName boost)
        : solenoid_ina(ina), solenoid_inb(inb), solenoid_en(en), solenoid_boost(boost), state(Idle) {
        solenoid_ina = 0;
        solenoid_inb = 0;
        solenoid_en = 0;
        solenoid_boost = 0;
        boost_charge_open_ms = DEFAULT_BOOST_CHARGE_OPEN_MS;
        boost_charge_close_ms = DEFAULT_BOOST_CHARGE_CLOSE_MS;
        hbridge_enable_delay_ms = DEFAULT_HBRIDGE_ENABLE_DELAY_MS;
        hbridge_enable_on_ms = DEFAULT_HBRIDGE_ENABLE_ON_MS;
    }

    ~Solenoid() {}

    /** Function for Open or Close Solenoid.
     *
     */
    void fire(Action act) {  // Action Open or Close
        if (state != Idle) {
            printf("Solenoid in action now. State is %i\n\r", state);
            return;
        }
        action = act;
        sendEvent(0);
    }

    void enable(int state) { solenoid_en = state; }

    void connect(int state) { solenoid_boost = state; }

    bool isValveOpened() { return valve_opened; }

    int get_solenoid_boost_charge_open_ms() { return boost_charge_open_ms; }

    void set_solenoid_boost_charge_open_ms(int ms) { boost_charge_open_ms = ms; }

    int get_solenoid_boost_charge_close_ms() { return boost_charge_close_ms; }

    void set_solenoid_boost_charge_close_ms(int ms) { boost_charge_close_ms = ms; }

    int get_solenoid_hbridge_enable_delay_ms() { return hbridge_enable_delay_ms; }

    void set_solenoid_hbridge_enable_delay_ms(int ms) { hbridge_enable_delay_ms = ms; }

    int get_solenoid_hbridge_enable_on_ms() { return hbridge_enable_on_ms; }

    void set_solenoid_hbridge_enable_on_ms(int ms) { hbridge_enable_on_ms = ms; }
};

#endif  //__HBRIDGE_H__
