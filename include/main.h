#ifndef MAIN_H /* include guards */
#define MAIN_H

#include "SimpleFSM.h"
#include "hw/temp_sensor.h"
#include "states/end.h"
#include "states/proc_start.h"
#include "states/start.h"

extern SimpleFSM fsm;
extern uint32_t uptime_counter;

enum triggers { start_proc_state = 1, next_state = 2, finished_state = 3, error_state = 4 };
#endif /* MAIN_H  */