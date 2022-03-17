#include "ProductionManager.h"

ProductionManager::ProductionManager() : state(State::PREPARE) {
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 4));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 2, 0));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 4, 0));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 6, 0));

	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 8, 0));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 10, 0));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 12, 0));

	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Overlord, 2, 9));
	//
	//int index = 7;
	//for (int i = 0; i < 100; ++i) {
	//	for (int j = 0; j < 8; ++j)
	//		buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 2 * (index++), 0));
	//}
}

void ProductionManager::update(int nActiveUnits) {
	if (frameDelay > 0) {
		--frameDelay;
		return;
	}

	if (state == State::ATTACK || state == State::ATTACKING) {
		if (nActiveUnits > 0 && state == State::ATTACK) {
			printf("switching to attacking state\n");
			state = State::ATTACKING;
		}
		else if (nActiveUnits == 0 && state == State::ATTACKING) {
			printf("switching to defend state\n");
			state = State::DEFEND;
			update(0);
			return;
		}
		const int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
		const int supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
		if (supplyUsed + BWAPI::UnitTypes::Zerg_Zergling.supplyRequired() >= supplyTotal) Tools::trainTroop(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
		else Tools::trainTroop(BWAPI::UnitTypes::Zerg_Zergling);
	}
	else if (state == State::PREPARE) {
		if (Tools::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool, Tools::getPoolPlacement())) {
			state = State::ATTACK;
			printf("switching to attack state\n");
		}
	}
	else {
		auto task_it = buildOrder.begin();
		while (task_it != buildOrder.end() && ((*task_it)->isDone() || (*task_it)->isInProgress())) {
			(*task_it)->updateState();
			++task_it;
		}

		if (task_it == buildOrder.end()) {
			// TODO: end of build order
			printf("End of build order\n");
		}
		else {
			(*task_it)->resolve();
		}
	}
	frameDelay = 5;
}

void ProductionManager::addTask(std::shared_ptr<Task>& task) {
	buildOrder.emplace_back(task);
}