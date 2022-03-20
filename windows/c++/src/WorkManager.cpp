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


	for (auto& unit : workers) {


		if (unit->isGatheringGas() || unit->isCarryingGas()) {
			n_gaz += 1;
		}
		if (unit->isGatheringMinerals()|| unit-> isCarryingMinerals()) {
			n_mine += 1;
		}
		if (unit->isIdle()) {
			std::cout << "yo" << std::endl;
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
				BWAPI::Unit closestGeyser = Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Extractor).value();
				if (closestGeyser) { unit->rightClick(closestGeyser); n_double += 1.0;}
			}



			if (prop_gaz <= n_double / N_double) {
				break;
			}
		}

	}
	else{
		for (auto& unit : workers) {
			if (unit->isGatheringGas()) {
				BWAPI::Unit closestMineral = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getMinerals()).value();
				if (closestMineral) { unit->rightClick(closestMineral);	n_double -= 1.0;
				}
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
