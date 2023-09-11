#include <Arduino.h>
#include "main.h"

void proc_start_entry()
{
    Serial.println("Entering State: process start");
}

void proc_start_ongoing()
{
    fsm.trigger(next_state);
}

void proc_start_exit()
{
    Serial.println("Exiting State: process start");
}