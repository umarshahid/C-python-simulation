#pragma once
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "SimulationObject.h"


class CoordinateSystem;

//class Waypoint : public SimulationObject
class Waypoint
{
    std::string name;
    std::string force; // "Red" or "Blue"
    CoordinateSystem& coordinateSystem; // Reference to a coordinate system
    float latitude, longitude; // Current position in lat/lon
    std::string iconPath;

public:
    // Constructor
    Waypoint(const std::string& name, const std::string& force, float startLatitude, float startLongitude, CoordinateSystem& coordinateSystem);

    // Getters
    std::string get_name() const;
    std::string get_force() const;
    std::pair<int, int> get_position() const;
    std::pair<int, int> get_position_xy() const;

    // Movement
    void update(); // Update function for simulation loop

};

