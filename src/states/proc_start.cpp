#include "main.h"
#include <Arduino.h>

char b[80];
// TODO: make it variabel
float tempEndRectRazgon = 90.0;

void proc_start_entry() {
  // TODO: close all valves and set power to max and messure the Atm pressure-
  Serial.println("Entering State: process start");
  // TODO: implementing send_message(b); // 00:00:00 acceleration to 90.0
  Serial.printf(b);
}

void proc_start_ongoing() {
  if (temperature_tube_bottom > fabs(tempEndRectRazgon)) {
    fsm.trigger(next_state);
  }
}

void proc_start_exit() {
  Serial.println(" ");
  Serial.println(" Exiting State: process start");
}