#pragma once

#include "Task.h"
#include <vector>
#include <memory>

class ProductionManager {
private:
	enum class State {
		PREPARE, ATTACK, DEFEND, ATTACKING
	};

	std::vector<std::shared_ptr<Task>> buildOrder;
	int frameDelay = 0;
	State state;
	
public:
	ProductionManager();

	void update(int nActiveUnits);
	void addTask(std::shared_ptr<Task>& task);
};