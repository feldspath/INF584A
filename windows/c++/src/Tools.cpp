#include "Tools.h"
#include <optional>

std::optional<BWAPI::Unit> Tools::getClosestUnitTo(const BWAPI::Position p, const BWAPI::Unitset& units) {
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : units) {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p)) {
            closestUnit = u;
        }
    }

    return closestUnit == nullptr ? std::nullopt : std::optional<BWAPI::Unit>(closestUnit);
}

std::optional<BWAPI::Unit> Tools::getClosestUnitTo(const BWAPI::Unit unit, const BWAPI::Unitset& units) {
    if (!unit) { return std::nullopt; }
    return getClosestUnitTo(unit->getPosition(), units);
}

int Tools::countUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units) {
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

std::optional<BWAPI::Unit> Tools::getIdleUnitOfType(BWAPI::UnitType type) {
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == type && unit->isIdle()) {
            return unit;
        }
    }

    return std::nullopt;
}

std::optional<BWAPI::Unit> Tools::getUnitOfType(BWAPI::UnitType type) {
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == type) {
            return unit;
        }
    }

    return std::nullopt;
}

std::optional<BWAPI::Unit> Tools::getIdleDepot()
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return getIdleUnitOfType(depot);
}

bool Tools::buildBuilding(BWAPI::UnitType type, BWAPI::TilePosition pos) {
    bool success = false;

    // check cost requirements
    const int minerals = BWAPI::Broodwar->self()->minerals();
    const int gas = BWAPI::Broodwar->self()->gas();
    if (minerals < type.mineralPrice() || gas < type.gasPrice()) return false;

    // Get the type of unit that is required to build the desired building
    const BWAPI::UnitType builderType = type.whatBuilds().first;

    // Get a unit that we own that is of the given type so it can build
    // If we can't find a valid builder unit, then we have to cancel the building
    BWAPI::Unitset builders;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == builderType && unit->isCompleted() && unit->canBuild()) 
            builders.insert(unit);
    }
    const auto builder = getClosestUnitTo(BWAPI::Position(pos), builders);
    if (!(builder.has_value())) {
        printf("no builder found\n");
        return false;
    }

    success = builder.value()->build(type, pos);

    // debug info
    if (success) printf("Started building %s\n", type.getName().c_str());
    else printf("Error while building %s\n", type.getName().c_str());

    return success;
}
// Attempt to construct a building of a given type 
bool Tools::buildBuilding(BWAPI::UnitType type) {
    const bool buildingOnCreep = type.requiresCreep();
    const auto desiredPos = BWAPI::Broodwar->self()->getStartLocation();
    const int maxBuildRange = 64;
    return buildBuilding(type, BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep));
}

void Tools::drawUnitCommands()
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        const BWAPI::UnitCommand & command = unit->getLastCommand();

        // If the previous command had a ground position target, draw it in red
        // Example: move to location on the map
        if (command.getTargetPosition() != BWAPI::Positions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        // If the previous command had a tile position target, draw it in red
        // Example: build at given tile position location
        if (command.getTargetTilePosition() != BWAPI::TilePositions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        // If the previous command had a unit target, draw it in red
        // Example: attack unit, mine mineral, etc
        if (command.getTarget() != nullptr)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }
    }
}

void Tools::drawUnitBoundingBoxes()
{
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

void Tools::smartRightClick(BWAPI::Unit unit, BWAPI::Unit target)
{
    // if there's no valid unit, ignore the command
    if (!unit || !target) { return; }

    // Don't issue a 2nd command to the unit on the same frame
    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount()) { return; }

    // If we are issuing the same type of command with the same arguments, we can ignore it
    // Issuing multiple identical commands on successive frames can lead to bugs
    if (unit->getLastCommand().getTarget() == target) { return; }
    
    // If there's nothing left to stop us, right click!
    unit->rightClick(target);
}

int Tools::getTotalSupply(bool inProgress)
{
    // start the calculation by looking at our current completed supplyt
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    // if we don't want to calculate the supply in progress, just return that value
    if (!inProgress) { return totalSupply; }

    // if we do care about supply in progress, check all the currently constructing units if they will add supply
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // ignore units that are fully completed
        if (unit->isCompleted()) { continue; }

        // if they are not completed, then add their supply provided to the total supply
        totalSupply += unit->getType().supplyProvided();
    }

    // one last tricky case: if a unit is currently on its way to build a supply provider, add it
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        // add the supply amount of the unit that it's trying to build
        totalSupply += command.getUnitType().supplyProvided();
    }

    return totalSupply;
}

void Tools::drawUnitHealthBars()
{
    // how far up from the unit to draw the health bar
    int verticalOffset = -10;

    // draw a health bar for each unit on the map
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        // determine the position and dimensions of the unit
        const BWAPI::Position& pos = unit->getPosition();
        int left = pos.x - unit->getType().dimensionLeft();
        int right = pos.x + unit->getType().dimensionRight();
        int top = pos.y - unit->getType().dimensionUp();
        int bottom = pos.y + unit->getType().dimensionDown();

        // if it's a resource, draw the resources remaining
        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            drawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        }
        // otherwise if it's a unit, draw the hp 
        else if (unit->getType().maxHitPoints() > 0)
        {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;
            drawHealthBar(unit, hpRatio, hpColor, 0);
            
            // if it has shields, draw those too
            if (unit->getType().maxShields() > 0)
            {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                drawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void Tools::drawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset)
{
    int verticalOffset = -10;
    const BWAPI::Position& pos = unit->getPosition();

    int left = pos.x - unit->getType().dimensionLeft();
    int right = pos.x + unit->getType().dimensionRight();
    int top = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop = top + yOffset + verticalOffset;
    int hpBottom = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i(left); i < right - 1; i += ticWidth)
    {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}

BWAPI::TilePosition Tools::getEnemyStartLocation() {
    const auto startLocs = BWAPI::Broodwar->getStartLocations();
    for (const auto& loc : startLocs) {
        const auto& startLoc = BWAPI::Broodwar->self()->getStartLocation();
        if (loc.x == startLoc.x && loc.y == startLoc.y) continue;
        return loc;
    }
}

bool Tools::trainTroop(BWAPI::UnitType unitType) {
    bool success = false;
    // check cost requirements
    const int minerals = BWAPI::Broodwar->self()->minerals();
    const int gas = BWAPI::Broodwar->self()->gas();
    if (minerals < unitType.mineralPrice() || gas < unitType.gasPrice()) return false;

    // zerg special case: when playing zerg, spawning pool is required but is not what builds the unit.
    if (BWAPI::Broodwar->self()->getRace().getID() == BWAPI::Races::Zerg.getID()) {
        const auto pool = BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool;
        if (!(Tools::getIdleUnitOfType(pool).has_value())) return false;
    }

    // builderUnitType =
    // Zergs: Larva
    // Terran: Barracks
    // Protoss: Gateway
    const auto builderUnitType = unitType.whatBuilds().first;
    const auto buildUnit = Tools::getIdleUnitOfType(builderUnitType);
    if (!(buildUnit.has_value())) return false;
    if (buildUnit.has_value() && buildUnit.value()->canTrain() && buildUnit.value()->train(unitType)) success = true;

    // debug info
    if (success) printf("Started training %s\n", unitType.getName().c_str());
    else printf("Error while training %s: %d/%d\n", unitType.getName().c_str(), BWAPI::Broodwar->self()->supplyUsed(), BWAPI::Broodwar->self()->supplyTotal());

    return success;
}

BWAPI::TilePosition Tools::getPoolPlacement() {
    const auto pos = BWAPI::Broodwar->self()->getStartLocation() + BWAPI::TilePosition(0, -2);
    const auto type = BWAPI::UnitTypes::Zerg_Spawning_Pool;
    const bool buildingOnCreep = type.requiresCreep();
    const auto desiredPos = BWAPI::Broodwar->self()->getStartLocation();
    const int maxBuildRange = 64;
    if (BWAPI::Broodwar->canBuildHere(pos, type)) return pos;
    else return BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
}

bool Tools::compareUnitTypes(BWAPI::UnitType t1, BWAPI::UnitType t2) {
    return t1.getID() == t2.getID();
}