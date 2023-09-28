#include "main.h"
#include "gui.h"
#include <Arduino.h>

SimpleFSM fsm;
uint32_t uptime_counter = 0;

State s[] = {State("start", start_entry, start_ongoing, start_exit),                        // 0
             State("process start", proc_start_entry, proc_start_ongoing, proc_start_exit), // 1
             State("warming up", NULL, NULL),                                               // 2
             State("initial stabilization", NULL, NULL),                                    // 3
             State("collection fraction", NULL, NULL),                                      // 4
             State("stabilization", NULL, NULL),                                            // 5
             State("process end", NULL, NULL),                                              // 6
             State("end", end_entry, end_ongoing, end_exit, true),                          // 7
             State("error", NULL, NULL, NULL, true)};                                       // 8

Transition transitions[] = {
    Transition(&s[0], &s[1], start_proc_state), Transition(&s[1], &s[2], next_state),
    Transition(&s[2], &s[3], next_state),       Transition(&s[3], &s[4], next_state),
    Transition(&s[4], &s[5], next_state),       Transition(&s[5], &s[6], next_state),
    Transition(&s[6], &s[7], next_state),       Transition(&s[0], &s[8], error_state),
    Transition(&s[1], &s[8], error_state),      Transition(&s[2], &s[8], error_state),
    Transition(&s[3], &s[8], error_state),      Transition(&s[4], &s[8], error_state),
    Transition(&s[5], &s[8], error_state),      Transition(&s[6], &s[8], error_state)};

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
