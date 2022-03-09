#pragma once

#include <BWAPI.h>

class Unit {

public:
	BWAPI::Unit unit;
	Unit(BWAPI::Unit _unit) : unit(_unit) {}
};