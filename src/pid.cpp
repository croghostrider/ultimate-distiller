/***************************************************************************
  sTune QuickPID Example (MAX31856, PTC Heater / SSR / Software PWM)
  This sketch does on-the-fly tuning and PID control. Tuning parameters are
  quickly determined and applied. During the initial temperature ramp-up,
  the output is reduced just prior to the input reaching setpoint. This
  reduces overshoot. View results using serial plotter.
  Reference: https://github.com/Dlloydev/sTune/wiki/Examples_MAX31856_PTC_SSR
  ***************************************************************************/
#include "main.h"
#include <QuickPID.h>
#include <sTune.h>

// pins
const uint8_t relayPin = 3;

// user settings
uint32_t settleTimeSec = 10;
uint32_t testTimeSec = 500; // runPid interval = testTimeSec / samples
const uint16_t samples = 500;
const float inputSpan = 200;
const float outputSpan = 100;
float outputStart = 0;
float outputStep = 50;
float tempLimit = 100;
uint8_t debounce = 1;
bool startup = true;

// variables
float Input, Output, Setpoint = 80, Kp, Ki, Kd;

sTune tuner = sTune(&Input, &Output, tuner.ZN_PID, tuner.directIP, tuner.printOFF);
QuickPID myPID(&Input, &Output, &Setpoint);

void setup24() {
  pinMode(relayPin, OUTPUT);

  tuner.Configure(inputSpan, outputSpan, outputStart, outputStep, testTimeSec, settleTimeSec,
                  samples);
  tuner.SetEmergencyStop(tempLimit);
}

void loop24() {
  switch (tuner.Run()) {
  case tuner.sample: // active once per sample during test
    Input = temperature_tube_bottom;
    tuner.plotter(Input, Output, Setpoint, 0.5f, 3); // output scale 0.5, plot every 3rd sample
    break;

  case tuner.tunings:                    // active just once when sTune is done
    tuner.GetAutoTunings(&Kp, &Ki, &Kd); // sketch variables updated by sTune
    myPID.SetOutputLimits(0, outputSpan * 0.1);
    myPID.SetSampleTimeUs((outputSpan - 1) * 1000);
    debounce = 0; // ssr mode
    Output = outputStep;
    myPID.SetMode(QuickPID::Control::automatic); // the PID is turned on
    myPID.SetProportionalMode(QuickPID::pMode::pOnMeas);
    myPID.SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);
    myPID.SetTunings(Kp, Ki, Kd); // update PID with the new tunings
    break;

  case tuner.runPid:                       // active once per sample after tunings
    if (startup && Input > Setpoint - 5) { // reduce overshoot
      startup = false;
      Output -= 9;
      myPID.SetMode(QuickPID::Control::manual);
      myPID.SetMode(QuickPID::Control::automatic);
    }
    Input = temperature_tube_bottom;
    myPID.Compute();
    break;
  }
}
