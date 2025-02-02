#include "Aircraft.h"
#include <iostream>
#include <cmath> // For sin and cos
#include "CoordinateSystem.h"
#include "FileLoader.cpp"
#include "Simulation.h"
#include "RenderManager.h"


// Constructor
Aircraft::Aircraft(const std::string& name, const std::string& force, int health,
    float startLatitude, float startLongitude, float heading, float speed, CoordinateSystem& coordinateSystem)
    : name(name), health(health), latitude(startLatitude), longitude(startLongitude),
    coordinateSystem(coordinateSystem), heading(heading), speed(speed), is_moving(false), id(nextID++) { // Default heading is North
    if (force != "Red" && force != "Blue") {
        std::cerr << "Invalid force value: " << force << ". Force must be 'Red' or 'Blue'.\n";
        this->force = "Blue"; // Default to Blue if invalid
    }
    else {
        this->force = force;
    }
    radar = Radar(150, 45.0f);
}

void Aircraft::adjustHeadingToNorth() {
    heading = 90.0f - heading;  // Adjust so 0 degrees points north
    if (heading < 0.0f) {
        heading += 360.0f;  // Ensure heading stays within 0-360 range
    }
}

// Getter Methods
int Aircraft::get_id() const {
    return id;
}

std::string Aircraft::get_name() const {
    return name;
}

std::string Aircraft::get_force() const {
    return force;
}

int Aircraft::get_health() const {
    return health;
}

float Aircraft::get_heading() const {
    return heading;
}

std::pair<float, float> Aircraft::get_position() const {
    return { latitude, longitude };
}

// Set the heading
void Aircraft::set_heading(float new_heading) {
    heading = std::fmod(new_heading, 360.0f); // Ensure heading is within 0-359 degrees
    if (heading < 0) {
        heading += 360.0f; // Normalize negative headings
    }
}


std::pair<int, int> Aircraft::get_position_xy() const {
    return coordinateSystem.to_screen_coordinates(latitude, longitude);
}

std::pair<int, int> Aircraft::get_target_position_xy() const {
    return coordinateSystem.to_screen_coordinates(target_latitude, target_longitude);
}

void Aircraft::move_to(float newLatitude, float newLongitude) {
    target_latitude = newLatitude;
    target_longitude = newLongitude;
    is_moving = true; // Start moving
}

void Aircraft::update_position() {
    if (!is_moving) return;

    // Calculate differences
    float delta_latitude = target_latitude - latitude;
    float delta_longitude = target_longitude - longitude;
    // Calculate the distance to the target
    float distance_to_target = std::sqrt(delta_latitude * delta_latitude +
        delta_longitude * delta_longitude);

    if (distance_to_target < speed) {
        // Close enough to the target, snap to target and stop moving
        latitude = target_latitude;
        longitude = target_longitude;
        is_moving = false;
    }
    else {
        // Move a step towards the target
        float ratio = speed / distance_to_target;
        latitude += delta_latitude * ratio;
        longitude += delta_longitude * ratio;

        // Update heading to point towards the target
        heading = std::atan2(delta_longitude, delta_latitude) * 180.0f / M_PI;
        // adjustHeadingToNorth();
    }
}

// Move the aircraft in the direction of its current heading
void Aircraft::move(float distance) {
    float heading_radians = heading * M_PI / 180.0f;

    // Calculate new latitude and longitude based on heading
    float delta_latitude = distance * std::cos(heading_radians);
    float delta_longitude = distance * std::sin(heading_radians);

    latitude += delta_latitude;
    longitude += delta_longitude;

    // Wrap around the coordinate system if needed
    coordinateSystem.wrap_coordinates(latitude, longitude);
}

void Aircraft::attack(Aircraft& target) {
    if (!is_alive()) {
        std::cout << name << " cannot attack because it is destroyed.\n";
        return;
    }
    if (!target.is_alive()) {
        std::cout << target.get_name() << " is already destroyed.\n";
        return;
    }
    target.health -= 10; // Example damage value
    target.health = std::max(0, target.health); // Ensure health does not go below 0
    std::cout << name << " attacked " << target.get_name() << "!\n";
}

void Aircraft::defend() {
    if (!is_alive()) {
        std::cout << name << " cannot defend because it is destroyed.\n";
        return;
    }
    health += 5; // Example defense healing value
    health = std::min(100, health); // Assume 100 is max health
    std::cout << name << " is defending and restored health to " << health << ".\n";
}

bool Aircraft::is_alive() const {
    return health > 0;
}

void Aircraft::perform_radar_scan() {
    auto screen_coordinates = coordinateSystem.to_screen_coordinates(latitude, longitude);
    int screen_x = screen_coordinates.first;
    int screen_y = screen_coordinates.second;

    auto& simulation = Simulation::get_instance();
    auto& allAircrafts = simulation.get_aircrafts_mutable();

    std::vector<std::reference_wrapper<Aircraft>> detectedEntities =
        radar.getEntitiesInRadarCone(allAircrafts, screen_x, screen_y, get_heading() - 90);

    for (const auto& entity : detectedEntities) {
        if (&entity.get() == this) // Skip self-detection
            continue;

        std::cout << "[" << get_name() << " - " << get_id() << "]'s radar detected: ["
            << entity.get().get_name() << " - " << entity.get().get_id() << "]\n";
    }
}

void Aircraft::update() {
    if (!is_alive()) {
        std::cout << name << " is destroyed and cannot perform updates.\n";
        return;
    }

    // Update the position if the aircraft is moving
    update_position();
	RenderManager::get_instance().drawAircraft(this);

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = currentTime - lastTime;
    lastTime = currentTime;

    deltaTime = delta.count(); // Delta time in seconds

    // Call function after 1 second
    static float elapsedTime = 0.0f;
    elapsedTime += deltaTime;
    if (elapsedTime >= 1.0f) {
        // #### Radar scane
        perform_radar_scan();
        // #### Radar scane
        elapsedTime = 0.0f;
    }
}