#pragma once

#include <BWAPI.h>

#include "Unit.h"

enum class WorkState {
	WORK, FLEE
};


class Worker : public Unit {

private:
	WorkState state;

public:
	Worker(BWAPI::Unit _unit) : Unit(_unit), state(WorkState::WORK) {}
};