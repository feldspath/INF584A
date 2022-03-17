#include "Unit.h"
#include <BWAPI.h>
#include <optional>

std::optional<BWAPI::Unit> Unit::getUnit() const {
	for (const auto& unit : BWAPI::Broodwar->getAllUnits()) {
		if (unit->getID() == id && unit->exists()) return unit;
	}
	return std::nullopt;
}