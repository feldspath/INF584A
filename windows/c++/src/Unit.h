#pragma once

#include <BWAPI.h>
#include <optional>

class Unit {
	int id;
public:
	Unit(BWAPI::Unit unit) : id(unit->getID()) {}

	std::optional<BWAPI::Unit> getUnit() const;
};