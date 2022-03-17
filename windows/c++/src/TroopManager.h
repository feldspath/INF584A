#pragma once

#include "Squad.h"
#include "Scout.h"
#include <vector>
#include <BWAPI.h>
#include <optional>

class TroopManager {
private:
	Squad squad;
	std::vector<Scout> scouts;

	int refreshOrder = 0;

	std::optional<BWAPI::Unit> getClosestHealingUnit() const;
	std::optional<BWAPI::Unit> getDangerousEnemyUnit() const;
	std::optional<BWAPI::Unit> getLowLifePreviousUnit() const;
	std::optional<BWAPI::Unit> getClosestOffensiveUnit() const;
	std::optional<BWAPI::Unit> getPreviousUnit() const;
	std::optional<BWAPI::Unit> getClosetWorker() const;
	std::optional<BWAPI::Unit> getClosetUnit() const;
	std::optional<BWAPI::Unit> getHealingSCV(BWAPI::Unit building) const;
	std::optional<BWAPI::Unit> getClosestGarbage() const;

public:
	int update();
	void addScout(BWAPI::Unit unit);
};