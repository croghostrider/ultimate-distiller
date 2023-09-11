#ifndef MAIN_H /* include guards */
#define MAIN_H

#include "SimpleFSM.h"
#include "states/start.h"
#include "states/proc_start.h"
#include "states/end.h"

extern SimpleFSM fsm;

enum triggers
{
    next_state = 1
};
#endif /* MAIN_H  */