#include "WorkManager.h"
#include "Tools.h"
#include <BWAPI.h>


void WorkManager::change_prop(double_t new_prop) {
	prop_gaz = new_prop;
}

void WorkManager::update() {
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
			//std::cout << "yo" << std::endl;
			const auto closestMineral = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
			if (closestMineral.has_value()) { unit->rightClick(closestMineral.value()); }
			n_mine += 1;
		}
	}

	if (n_gaz + n_mine == 0) return;

	const double_t N_double = static_cast<double_t>(n_gaz + n_mine);
	double_t n_double = static_cast<double_t>(n_gaz);


	if (n_double / N_double <= prop_gaz && prop_gaz < (n_double + 1.0) / N_double) {
		return;
	}

	if (n_double / N_double <= prop_gaz) {
		for (auto& unit : workers) {
			if (unit->isGatheringMinerals()) {
				const auto closestGeyser = Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Extractor);
				if (closestGeyser.has_value()) { unit->rightClick(closestGeyser.value()); n_double += 1.0;}
			}

			if (prop_gaz <= n_double / N_double) {
				break;
			}
		}
	}
	else {
		for (auto& unit : workers) {
			if (unit->isGatheringGas()) {
				const auto closestMineral = Tools::getClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
				if (closestMineral.has_value()) { unit->rightClick(closestMineral.value());	n_double -= 1.0;
				}
			}

			if (n_double / N_double <= prop_gaz) {
				break;
			}
		}
	}
}

void WorkManager::addWorker(BWAPI::Unit unit) {
	workers.emplace_back(unit);
}
