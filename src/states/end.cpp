#include <Arduino.h>
#include "main.h"

void end_entry()
{
    Serial.println("Entering State: end");
}

void end_ongoing()
{
}

void end_exit()
{
    fsm.setFinishedHandler(finished);
    Serial.println("Exiting State: end");
}

void finished()
{
    Serial.println("THE END!");
}