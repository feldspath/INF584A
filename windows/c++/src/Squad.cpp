#pragma once

#include "Squad.h"
#include <BWAPI.h>

void Squad::addUnit(const BWAPI::Unit& unit) {
	troops.emplace_back(unit, id);
}
