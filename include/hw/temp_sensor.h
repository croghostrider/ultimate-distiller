#ifndef TEMP_SENSOR_H /* include guards */
#define TEMP_SENSOR_H
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor_heating_element = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_tube_bottom = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_tube_top = Adafruit_MCP9808();

extern float temperature_heating_element;
extern float temperature_tube_bottom;
extern float temperature_tube_top;

/* init temperature sensors function */
void int_sensor();

/* read temperature sensors function */
void read_sensor();

void initializeSensor(Adafruit_MCP9808 &sensor, int address);

#endif /* TEMP_SENSOR_H  */