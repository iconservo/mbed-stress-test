/*
 * This file contains state machine function for work with Solenoid,
 *
 */

#include "hbridge.h"

// State machine for open or close solenoid
void Solenoid::sequencer() {
    switch (state) {
        // Start state (Charge boost)
        case Idle:
            if (event != Start) {
                break;
            }
            solenoid_boost = 1;
            solenoid_en = 0;
            solenoid_ina = 0;
            solenoid_inb = 0;
            state = BoostLatched;
            event = En_bridge;
            if (action == Open) {
                sendEvent(boost_charge_open_ms);
            } else {
                sendEvent(boost_charge_close_ms);
            }
            break;

        // State when cap is charged (Enable hbridge)
        case BoostLatched:
            solenoid_boost = 0;
            solenoid_en = 1;
            state = HbridgeEnableDelay;
            event = En_bridge;
            sendEvent(hbridge_enable_delay_ms);
            break;

        // State when h-bridge enabled (Open or Close solenoid)
        case HbridgeEnableDelay:
            if (action == Open) {
                solenoid_ina = 1;
                solenoid_inb = 0;
            } else {
                solenoid_ina = 0;
                solenoid_inb = 1;
            }
            state = Pulse;
            event = Dis_bridge;
            sendEvent(hbridge_enable_on_ms);
            break;

        // State after action (Disadle h-bridge)
        case Pulse:
            if (action == Open) {
                valve_opened = 1;
            } else {
                valve_opened = 0;
            }
            solenoid_en = 0;
            solenoid_boost = 0;
            solenoid_ina = 0;
            solenoid_inb = 0;
            state = Idle;
            event = Start;
            break;
    }
}
