#include "Waypoint.h"
#include <iostream>
#include <cmath> // For sin and cos
#include "CoordinateSystem.h"
#include "FileLoader.cpp"
#include "RenderManager.h"

Waypoint::Waypoint(const std::string& name, const std::string& force,
    float startLatitude, float startLongitude, CoordinateSystem& coordinateSystem)
    : name(name), latitude(startLatitude), longitude(startLongitude),
    coordinateSystem(coordinateSystem) { // Default heading is North
    if (force != "Red" && force != "Blue") {
        std::cerr << "Invalid force value: " << force << ". Force must be 'Red' or 'Blue'.\n";
        this->force = "Blue"; // Default to Blue if invalid
    }
    else {
        this->force = force;
    }
    //iconPath = FileLoader::getSimulationObjectTexture(SimulationObjectType::Waypoint);
}

std::string Waypoint::get_name() const {
    return name;
}

std::string Waypoint::get_force() const {
    return force;
}

std::pair<int, int> Waypoint::get_position() const {
    return { latitude, longitude };
}

std::pair<int, int> Waypoint::get_position_xy() const {
    return coordinateSystem.to_screen_coordinates(latitude, longitude);
}

void Waypoint::update() {
    RenderManager::get_instance().drawWaypoint(this);
}
