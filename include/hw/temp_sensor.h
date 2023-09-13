#ifndef TEMP_SENSOR_H /* include guards */
#define TEMP_SENSOR_H
#include "Adafruit_MCP9808.h"

extern float temperature_heating_element;
extern float temperature_tube_bottom;
extern float temperature_tube_top;

/* read temperature sensors function */
void read_sensor();

/* init temperature sensors function */
void initializeTemperatureSensors();

void initializeSensor(Adafruit_MCP9808 &sensor, int address);

#endif /* TEMP_SENSOR_H  */