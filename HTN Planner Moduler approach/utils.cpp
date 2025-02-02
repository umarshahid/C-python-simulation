#pragma once
#include <string>
#include "enums.h"

static std::string simulationObjectTypeToString(SimulationObjectType type) {
    switch (type) {
    case SimulationObjectType::Aircraft: return "Aircraft";
    case SimulationObjectType::Waypoint: return "Waypoint";
    case SimulationObjectType::Missile: return "Missile";
    default: return "Unknown";
    }
}