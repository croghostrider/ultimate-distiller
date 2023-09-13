#include "main.h"
#include "gui.h"
#include <Arduino.h>

SimpleFSM fsm;
uint32_t uptime_counter = 0;

State s[] = {State("start", start_entry, start_ongoing, start_exit),
             State("process start", proc_start_entry, proc_start_ongoing,
                   proc_start_exit),
             State("warming up", NULL, NULL),
             State("initial stabilization", NULL, NULL),
             State("collection fraction", NULL, NULL),
             State("stabilization", NULL, NULL),
             State("process end", NULL, NULL),
             State("end", end_entry, end_ongoing, end_exit, true)};

Transition transitions[] = {Transition(&s[0], &s[1], next_state),
                            Transition(&s[1], &s[7], next_state)};

int num_transitions = sizeof(transitions) / sizeof(Transition);

void setup() {
  randomSeed(0);
  Serial.begin(115200);
  while (!Serial) {
  }
  if (SLOW_BOOT) {
    delay(5000); // Delay booting to give time to connect a serial monitor
  }
  connectWifi();
  WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
                        // responsivness (at the cost of power use)
  setUpUI();

  fsm.add(transitions, num_transitions);
  fsm.setInitialState(&s[0]);
}

void loop() {
  uiLoop();
  fsm.run(1000);
}
