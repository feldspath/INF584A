#pragma once

#include "Unit.h"
#include <set>
#include <vector>
#include <optional>

class Squad {
private:
	float mergeDistance = 128;
	float hostileDistance = 800;

	BWAPI::Position center;
	std::set<int> troops;
	std::optional<Unit> previousTarget = std::nullopt;
	std::optional<BWAPI::Unit> previousTargetUnit;

public:
	void addUnit(const BWAPI::Unit& unit);
	int update();
	bool tryAddUnit(const BWAPI::Unit& unit);
	BWAPI::Position getCenter() const;
	BWAPI::Unitset getTroops() const;
	std::optional<BWAPI::Unit> getPreviousUnit() const;
	void updatePreviousTarget(std::optional<BWAPI::Unit> target);

	// returns total damage per frame
	float getDPSEstimate() const;
};