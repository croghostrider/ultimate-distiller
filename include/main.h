#ifndef MAIN_H /* include guards */
#define MAIN_H

#include "SimpleFSM.h"
#include "states/start.h"
#include "states/proc_start.h"
#include "states/end.h"
#include "hw/temp_sensor.h"

extern SimpleFSM fsm;
extern uint32_t uptime_counter;

enum triggers
{
    next_state = 1
};
#endif /* MAIN_H  */