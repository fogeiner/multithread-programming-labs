#pragma once


#include "SwitchControlled.h"
#include "../AsyncDispatcher/Async.h"

class SwitchControlledAsync : public SwitchControlled, public Async {
};