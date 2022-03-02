#pragma once

#include <BWAPI.h>

#include "Unit.h"

enum class TroopState {
	FIGHT, FLEE, BAIT, SCOUT, DEFEND, IDLE
};


class Troop : public Unit {

private:
	TroopState state;
	int squadId;

public:
	Troop(BWAPI::Unit _unit, int _squadId) : Unit(_unit), squadId(_squadId), state(TroopState::IDLE) {}
};