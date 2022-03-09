#include "ProductionManager.h"

ProductionManager::ProductionManager() {
	/*buildOrder.reserve(6);
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 1, 9));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Barracks, 1, 11));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Barracks, 2, 13));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 2, 15));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Refinery, 1, 16));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 3, 17));*/

	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 4));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 1, 4));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 2, 4));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 3, 4));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Overlord, 2, 9));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Hatchery, 2, 9));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 4, 8));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 5, 8));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 6, 8));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Extractor, 1, 11));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Lair, 1, 13));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 7, 13));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 8, 13));
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 9, 13));

	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 4));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 2, 0));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 4, 0));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 6, 0));

	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 8, 0));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 10, 0));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 12, 0));

	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Overlord, 2, 9));
	
	int index = 7;
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 8; ++j)
			buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Zergling, 2 * (index++), 0));
	}
}

void ProductionManager::update() {
	if (frameDelay > 0) {
		--frameDelay;
		return;
	}
	if (buildOrder.size() == 0) return;
	auto task_it = buildOrder.begin();
	while (task_it != buildOrder.end() && (*task_it)->isDone()) {
		printf("task finished\n");
		buildOrder.erase(task_it);
		task_it = buildOrder.begin();
	}
	while (task_it != buildOrder.end() && (*task_it)->isInProgress()) ++task_it;
	if (task_it == buildOrder.end()) {
		BWAPI::Broodwar->printf("End of build order");
	}
	else {
		(*task_it)->resolve();
		frameDelay = 5;
	}
}

void ProductionManager::addTask(std::shared_ptr<Task>& task) {
	buildOrder.emplace_back(task);
}