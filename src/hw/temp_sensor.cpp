#include "hw/temp_sensor.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor_heating_element = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_tube_bottom = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_tube_top = Adafruit_MCP9808();

float temperature_heating_element;
float temperature_tube_bottom;
float temperature_tube_top;

void read_sensor() {
  temperature_heating_element = tempsensor_heating_element.readTempC();
  Serial.print("Heating element Temp: ");
  Serial.print(temperature_heating_element, 4);
  Serial.println("*C\t");

  temperature_tube_bottom = tempsensor_tube_bottom.readTempC();
  Serial.print("Tube botton Temp: ");
  Serial.print(temperature_tube_bottom, 4);
  Serial.println("*C\t");

  temperature_tube_top = tempsensor_tube_top.readTempC();
  Serial.print("Tube botton Temp: ");
  Serial.print(temperature_tube_top, 4);
  Serial.println("*C\t");
}
void initializeTemperatureSensors() {
  initializeSensor(tempsensor_heating_element, 0x18);
  initializeSensor(tempsensor_tube_bottom, 0x19);
  initializeSensor(tempsensor_tube_top, 0x1A);
}

void initializeSensor(Adafruit_MCP9808 &sensor, int address) {
  if (!sensor.begin(address)) {
    Serial.print("Couldn't find MCP9808! Check your connections and verify the address: 0x");
    Serial.print(address, HEX);
    Serial.println(" is correct.");
  } else {
    sensor.wake(); // wake up, ready to read!
    sensor.setResolution(3);
  }
}