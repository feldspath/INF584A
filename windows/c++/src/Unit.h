#pragma once

#include <BWAPI.h>

class Unit {
protected:
	BWAPI::Unit unit;

public:
	Unit(BWAPI::Unit _unit) : unit(_unit) {}
};