#include "WorkManager.h"
#include "Tools.h"
#include <BWAPI.h>


void WorkManager::change_prop(size_t new_prop)
{
	prop_gaz = new_prop;
}

void WorkManager::update()
{
	size_t n_gaz = 0;
	size_t n_mine = 0;

	for (auto& unit : workers) {
		if (unit->isGatheringGas()) {
			n_gaz += 1;
		}
		if (unit->isGatheringMinerals()) {
			n_mine += 1;
		}
		if (unit->isIdle()) {
			BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
			if (closestMineral) { unit->rightClick(closestMineral); }
			n_mine += 1;
		}
	}


	size_t N = n_gaz + n_mine;

	if (n_gaz / N <= prop_gaz < (n_gaz + 1) / N) {
		return;
	}

	if (n_gaz / N <= prop_gaz) {
		for (auto& unit : workers) {
			if (unit->isGatheringMinerals()) {
				BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
				if (closestMineral) { unit->rightClick(closestMineral); }
				n_gaz += 1;
			}

			if (prop_gaz <= n_gaz / N) {
				break;
			}
		}

	}
	else{
		for (auto& unit : workers) {
			if (unit->isGatheringMinerals()) {
				BWAPI::Unit closestGeyser = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getGeysers());
				if (closestGeyser) { unit->rightClick(closestGeyser); }
				n_gaz += 1;
			}

			if (n_gaz / N <= prop_gaz) {
				break;
			}
		}

	}
	

}

void WorkManager::addWorker(BWAPI::Unit unit)
{
	workers.emplace_back(unit);
}
