#include "WorkManager.h"
#include "Tools.h"
#include <BWAPI.h>


void WorkManager::change_prop(double_t new_prop)
{
	prop_gaz = new_prop;
}

void WorkManager::update()
{


	size_t n_gaz = 0;
	size_t n_mine = 0;


	if (state == WorkState::FLEE) {
		const auto botBase = BWAPI::Broodwar->self()->getStartLocation();
		BWAPI::TilePosition off = { 60, -5 };
		botBase + BWAPI::TilePosition(-off.x, -off.y);
		for (auto& unit : workers) {
			unit->move(BWAPI::Position(4600, 56));
		}
		return;
		
	}

	for (auto& unit : workers) {


		if (unit->isGatheringGas() || unit->isCarryingGas()) {
			n_gaz += 1;
		}
		if (unit->isGatheringMinerals()|| unit-> isCarryingMinerals()) {
			n_mine += 1;
		}
		if (unit->isIdle()) {
			BWAPI::Unit closestMineral = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getMinerals()).value();
			if (closestMineral) { unit->rightClick(closestMineral); }
			n_mine += 1;
		}
	}


	size_t N = n_gaz + n_mine;

	if (N == 0) return;

	double_t N_double = (double_t)N;
	double_t n_double = (double_t)n_gaz;


	if (n_double / N_double <= prop_gaz && prop_gaz < (n_double + 1.0) / N_double) {
		return;
	}

	if (n_double / N_double <= prop_gaz) {
		for (auto& unit : workers) {
			if (unit->isGatheringMinerals()) {
				BWAPI::Unit closestMineral = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getMinerals()).value();
				if (closestMineral) { unit->rightClick(closestMineral); }
				n_gaz += 1;
			}



			if (prop_gaz <= n_double / N_double) {
				break;
			}
		}

	}
	else{
		for (auto& unit : workers) {
			if (unit->isGatheringMinerals()) {
				BWAPI::Unit closestGeyser = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getGeysers()).value();
				if (closestGeyser) { unit->rightClick(closestGeyser); }
				n_gaz += 1;
			}

			if (n_double / N_double <= prop_gaz) {
				break;
			}
		}

	}

	
	

}

void WorkManager::addWorker(BWAPI::Unit unit)
{
	workers.emplace_back(unit);
}
