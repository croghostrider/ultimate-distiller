#include "main.h"
#include <Arduino.h>

void start_entry() {
  // TODO: close all valves and set power to 0
  Serial.println("Entering State: start");
  initializeTemperatureSensors();
}

void start_ongoing() {
  // TODO: check the sensors
  Serial.print(".");
}

void start_exit() {
  Serial.println(" ");
  Serial.println(" Exiting State: start");
}