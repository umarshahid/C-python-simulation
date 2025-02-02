#pragma once

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "SimulationObject.h"
#include "Radar.h"
#include <chrono>


class CoordinateSystem;
class Simulation;

//class Aircraft : public SimulationObject {
class Aircraft {
private:
    static int nextID;
    int id;
    std::string name;
    std::string force; // "Red" or "Blue"
    int health;
    float heading; // Heading in degrees (0 = North)
    CoordinateSystem& coordinateSystem; // Reference to a coordinate system
    float latitude, longitude; // Current position in lat/lon
    float target_latitude=0; 
    float target_longitude=0; // Target position in lat/lon
    float speed; // Speed in lat/lon units per update

    bool is_moving; // Flag to indicate if the aircraft is moving
    std::string iconPath;

    Radar radar;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;

public:
    // Constructor
    Aircraft(const std::string& name, const std::string& force, int health,
        float startLatitude, float startLongitude, float heading, float speed, CoordinateSystem& coordinateSystem);

    // Getters
    int get_id() const;
    std::string get_name() const;
    std::string get_force() const;
    int get_health() const;
    float get_heading() const;
    std::pair<float, float> get_position() const;
    std::pair<int, int> get_position_xy() const;
	std::pair<int, int> get_target_position_xy() const;

    // Movement
    void move_to(float newLatitude, float newLongitude); // Initiate movement
    void move(float distance);
    void update(); // Update function for simulation loop
    void attack(Aircraft& target);
    void defend();
    bool is_alive() const;
    void adjustHeadingToNorth();
    void set_heading(float new_heading);
    void perform_radar_scan();
	inline bool get_isMoving() { return is_moving; }
	inline Radar* getRadar() { return &radar; }

private:
    void update_position(); // Gradually move towards the target

};
