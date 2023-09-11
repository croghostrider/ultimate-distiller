#include <Arduino.h>
#include "main.h"

void start_entry()
{
    // TODO: close all valves and set power to 0
    Serial.println("Entering State: start");
}

void start_ongoing()
{
    // TODO: check the sensors
    fsm.trigger(next_state);
}

void start_exit()
{
    Serial.println("Exiting State: start");
}