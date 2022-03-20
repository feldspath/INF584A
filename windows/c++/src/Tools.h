#pragma once

#include <BWAPI.h>
#include <optional>

namespace Tools
{
    std::optional<BWAPI::Unit> getClosestUnitTo(const BWAPI::Position p, const BWAPI::Unitset& units);
    std::optional<BWAPI::Unit> getClosestUnitTo(const BWAPI::Unit unit, const BWAPI::Unitset& units);

    int countUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    std::optional<BWAPI::Unit> getIdleUnitOfType(BWAPI::UnitType type);
    std::optional<BWAPI::Unit> getUnitOfType(BWAPI::UnitType type);
    std::optional<BWAPI::Unit> getIdleDepot();

    bool buildBuilding(BWAPI::UnitType type, BWAPI::TilePosition pos);
    bool buildBuilding(BWAPI::UnitType type);

    void drawUnitBoundingBoxes();
    void drawUnitCommands();

    void smartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int getTotalSupply(bool inProgress = false);

    void drawUnitHealthBars();
    void drawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);

    bool trainTroop(BWAPI::UnitType);
    void createWorker();

    BWAPI::TilePosition getEnemyStartLocation();
    BWAPI::TilePosition getPoolPlacement();

    bool compareUnitTypes(BWAPI::UnitType t1, BWAPI::UnitType t2);
}