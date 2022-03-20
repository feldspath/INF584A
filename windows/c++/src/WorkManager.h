#pragma once

#include "Worker.h"
#include <vector>
#include <BWAPI.h>


class WorkManager {
private:

	std::vector<BWAPI::Unit> workers;
	
	double_t prop_gaz = 0.0;

	WorkState state = WorkState::WORK;


public:
	void change_prop(double_t new_prop);
	void update();
	void addWorker(BWAPI::Unit unit);
};