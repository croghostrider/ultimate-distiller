#include <Arduino.h>
#include "main.h"

State s[] = {
    State("start", start_entry, start_ongoing, start_exit),
    State("process start", proc_start_entry, proc_start_ongoing, proc_start_exit),
    State("warming up", NULL, NULL),
    State("initial stabilization", NULL, NULL),
    State("collection fraction", NULL, NULL),
    State("stabilization", NULL, NULL),
    State("process end", NULL, NULL),
    State("end", end_entry, end_ongoing, end_exit, true)};

Transition transitions[] = {
    Transition(&s[0], &s[1], next_state),
    Transition(&s[1], &s[7], next_state)};

int num_transitions = sizeof(transitions) / sizeof(Transition);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(300);
  }
  fsm.add(transitions, num_transitions);
  fsm.setInitialState(&s[0]);
}

void loop()
{
  fsm.run(1000);
}
