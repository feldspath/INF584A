#pragma once

#include "Worker.h"
#include <vector>
#include <BWAPI.h>

class WorkManager {
private:
	std::vector<BWAPI::Unit> workers;
	
	size_t prop_gaz = 0;

public:
	void change_prop(size_t new_prop);
	void update();
	void addWorker(BWAPI::Unit unit);
};