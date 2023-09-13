#include <RBDdimmer.h>
#include "config.h"

dimmerLamp dimmer(outputPin, zerocross); // initialase port for dimmer for ESP8266, ESP32, Arduino due boards

int outVal = 0;

void setup2()
{
    dimmer.begin(NORMAL_MODE, OFF); // dimmer initialisation: name.begin(MODE, STATE)
}

void loop2()
{
    // outVal = map(analogRead(0), 1, 1024, 100, 0); // analogRead(analog_pin), min_analog, max_analog, 100%, 0%);
    dimmer.setPower(outVal); // name.setPower(0%-100%)
}