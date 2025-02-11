#pragma once

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "SimulationObject.h"
#include "Radar.h"
#include <chrono>
#include "utils.cpp"
#include <algorithm>
#include "Missile.h"


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
    float latitude, longitude, altitude;// Current position in lat/lon
    float target_latitude=0; 
    float target_longitude=0; // Target position in lat/lon
    float speed; // Speed in lat/lon units per update
    Vector3 position;
    Vector3 velocity;

    bool is_moving; // Flag to indicate if the aircraft is moving
    std::string iconPath;

    Radar radar;

    //const Vector3 &current_target_position;
    //const Vector3 &current_target_velocity;

    Aircraft* current_target = nullptr;  // Pointer to the currently engaged target


    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;

public:
    // Constructor
    Aircraft() = default; // Ensure a default constructor is available
    Aircraft(const std::string& name, const std::string& force, int health,
        float startLatitude, float startLongitude, float heading, float speed, CoordinateSystem& coordinateSystem);

    // Deleted copy constructor and copy assignment operator to avoid copying
    //Aircraft(const Aircraft&) = delete;
    //Aircraft& operator=(const Aircraft&) = delete;

    //// Move constructor and move assignment operator if needed
    //Aircraft(Aircraft&& other) noexcept;
    //Aircraft& operator=(Aircraft&& other) noexcept;

    //~Aircraft();
    // Getters
    int get_id() const;
    std::string get_name() const;
    std::string get_force() const;
    int get_health() const;
    float get_heading() const;
    Vector3 get_velocity() const;
    Vector3 get_position3() const;
    std::pair<float, float> get_position() const;
    std::pair<int, int> get_position_xy() const;
	std::pair<int, int> get_target_position_xy() const;

    // Movement
    void move_to(float newLatitude, float newLongitude); // Initiate movement
    void move(float distance);
    void update(double dt); // Update function for simulation loop
    void attack(Aircraft& target);
    void defend();
    bool is_alive() const;
    void adjustHeadingToNorth();
    void set_heading(float new_heading);
    void perform_radar_scan();
	inline bool get_isMoving() { return is_moving; }
	inline Radar* getRadar() { return &radar; }

    // Missile management
    Missile* missile{nullptr};  // Pointer to missile (single missile for simplicity)

    void launch_missile();
    void update_missile(double dt, Vector3 targetPos, Vector3 targetVel);

private:
    void update_position(double dt); // Gradually move towards the target
    Vector3 calculateVelocityFromAirspeed(double airspeed, double heading, double climbAngle);

};
