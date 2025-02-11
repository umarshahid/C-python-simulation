#include "Missile.h"
#include "RenderManager.h"

// Constructor
Missile::Missile(float heading, std::string force, Vector3 launchPos, Vector3 launchVel, double missileSpeed, double maxAccel)
    : heading(heading), force(force), position(launchPos), velocity(launchVel), speed(missileSpeed), maxAcceleration(maxAccel), active(true) {
}

// Update Function
void Missile::update(Vector3 targetPos, Vector3 targetVel, double dt) {
    if (!active) return;

    _targetPos = targetPos;

    updatePosition(targetPos, targetVel, dt);
    RenderManager::get_instance().drawMissile(this);
}

void Missile::updatePosition(const Vector3& targetPos, const Vector3& targetVel, double dt) {
    if (!active) return;

    // Target position
    // Compute the target heading (direction to target)
    Vector3 direction = targetPos - position;
    float target_heading = std::atan2(direction.y, direction.x) * 180.0f / M_PI + 90.0f;

    // Smoothly rotate towards target heading
    float rotation_speed = 30.0f * dt;  // Adjust rotation speed based on dt
    float heading_diff = target_heading - heading;

    // Normalize the heading difference to range [-180, 180]
    if (heading_diff > 180.0f) heading_diff -= 360.0f;
    if (heading_diff < -180.0f) heading_diff += 360.0f;

    // Clamp the change in heading
    heading += std::clamp(heading_diff, -rotation_speed, rotation_speed);

    // Ensure heading remains within [-180, 180]
    if (heading > 180.0f) heading -= 360.0f;
    if (heading < -180.0f) heading += 360.0f;

    // Move **in the direction of the current heading**, not directly to the target
    float move_speed = 30.0f * dt; // Adjust movement speed
    float heading_rad = (heading - 90.0f) * M_PI / 180.0f; // Convert degrees to radians

    // Move in the heading direction
    position.x += move_speed * std::cos(heading_rad);
    position.y += move_speed * std::sin(heading_rad);

    // Stop when close to the target
    if ((position - targetPos).magnitude() < 1.5f) {
        position = targetPos;
        active = false;
		hit = true;
        std::cout << "reached.......";
    }

}

Vector3 Missile::get_position3() const {
    return position;
}

double Missile::getHeading() {
    double heading = std::atan2(velocity.y, velocity.x) * 180.0 / M_PI; // Convert to degrees
    if (heading < 0) {
        heading += 360.0;  // Ensure heading is in [0, 360] range
    }
    return heading;
}

float Missile::get_heading() {
    return heading;
}

void Missile::set_heading(float new_heading) {
    heading = std::fmod(new_heading, 360.0f); // Ensure heading is within 0-359 degrees
    if (heading < 0) {
        heading += 360.0f; // Normalize negative headings
    }
}