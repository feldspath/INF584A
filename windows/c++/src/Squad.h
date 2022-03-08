#pragma once

#include "Troop.h"
#include <vector>

class Squad {
private:
	int id;
	std::vector<Troop> troops;

public:
	const std::vector<Troop>& getTroops() const { return troops; }
	void addUnit(const BWAPI::Unit& unit);
};