#include "Scout.h"

int Scout::count = 0;

void Scout::update() {
	const auto u = this->getUnit();
	if (!(u.has_value())) return;
	if (u.value()->getDistance(scoutPositions.at(target)) < 10) {
		++target;
		if (target >= scoutPositions.size()) target = 0;
	}
	u.value()->move(scoutPositions.at(target));
}

static std::vector<BWAPI::TilePosition> baseOffsets = {
	{30, 5}, {60, -5}, {60, 20}, {60, 60}, {55, 90}
};

Scout::Scout(const BWAPI::Unit& unit) : Unit(unit) {
	target = count % 2 == 0 ? 0 : baseOffsets.size();
	++count;
	const auto enemyBase = Tools::getEnemyStartLocation();
	const auto botBase = BWAPI::Broodwar->self()->getStartLocation();

	const bool invert = enemyBase.x < botBase.x;
	for (const auto off : baseOffsets) {
		scoutPositions.emplace_back(botBase + (invert ? BWAPI::TilePosition(-off.x, -off.y) : off));
	}
	scoutPositions.emplace_back(botBase);
	for (auto it = baseOffsets.rbegin(); it != baseOffsets.rend(); ++it) {
		scoutPositions.emplace_back(enemyBase + (invert ? *it : BWAPI::TilePosition(-(it->x), -(it->y))));
	}
	scoutPositions.emplace_back(botBase);
}