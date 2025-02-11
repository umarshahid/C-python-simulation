#pragma once
#include "INIReader.h"
#include "enums.h"
#include "iostream"


class FileLoader {

public:
    static std::string getButtonTexture(SimulationObjectType buttonType) {
        INIReader reader("../assets/files/paths.ini");

        if (reader.ParseError() < 0) {
            std::cerr << "Error: Unable to load 'paths.ini'\n";
            return "default_value";
        }

        switch (buttonType) {
        case SimulationObjectType::Aircraft:
            return reader.Get("BUTTON_ICONS", "aircraft", "default_value");
        case SimulationObjectType::Waypoint:
            return reader.Get("BUTTON_ICONS", "waypoint", "default_value");
        case SimulationObjectType::Unknown:
        default:
            return reader.Get("BUTTON_ICONS", "plan", "default_value");
        }
    }

    static std::string getSimulationObjectTexture(SimulationObjectType buttonType) {
        INIReader reader("../assets/files/paths.ini");

        if (reader.ParseError() < 0) {
            std::cerr << "Error: Unable to load 'paths.ini'\n";
            return "default_value";
        }

        switch (buttonType) {
        case SimulationObjectType::Aircraft:
            return reader.Get("OBJECT_ICONS", "aircraft", "default_value");
        case SimulationObjectType::Waypoint:
            return reader.Get("OBJECT_ICONS", "waypoint", "default_value");
        case SimulationObjectType::Missile:
            return reader.Get("OBJECT_ICONS", "missile", "default_value");
        case SimulationObjectType::Unknown:
        default:
            return reader.Get("OBJECT_ICONS", "plan", "default_value");
        }
    }
};