#include "Radar.h"
#include "Aircraft.h"


Radar::Radar() : radarRadius(100), radarAngle(45.0f) {

}

Radar::Radar(int radius, float angle) : radarRadius(radius), radarAngle(angle) {

}


std::vector<std::reference_wrapper<Aircraft>> Radar::getEntitiesInRadarCone(std::vector<std::unique_ptr<Aircraft>>& entities, int centerX, int centerY, float heading) {
    std::vector<std::reference_wrapper<Aircraft>> entitiesInCone;

    // Convert heading and angles to radians
    float radarHeadingRad = normalizeAngle(heading * M_PI / 180.0f);
    float leftBound = normalizeAngle(radarHeadingRad - (radarAngle * M_PI / 180.0f));
    float rightBound = normalizeAngle(radarHeadingRad + (radarAngle * M_PI / 180.0f));

    for (auto& entity : entities) {
        // Calculate the vector from radar center to the entity
        std::pair<int, int> screen_coordinates = entity->get_position_xy();
        int dx = screen_coordinates.first - centerX;
        int dy = screen_coordinates.second - centerY;

        // Calculate the distance from the center
        float distance = std::sqrt(dx * dx + dy * dy);

        // Ignore entities outside the radar radius
        if (distance > radarRadius) {
            continue;
        }

        // Calculate the angle of the entity relative to the radar center
        float entityAngle = std::atan2(dy, dx);
        entityAngle = normalizeAngle(entityAngle);

        // Check if the entity is within the cone's angle range
        bool isInCone = false;
        if (leftBound <= rightBound) {
            isInCone = (entityAngle >= leftBound && entityAngle <= rightBound);
        }
        else {
            // Handle cone crossing 0/2π boundary
            isInCone = (entityAngle >= leftBound || entityAngle <= rightBound);
        }

        if (isInCone) {
            entitiesInCone.push_back(*entity);
        }
    }

    return entitiesInCone;

}

// Helper function to normalize an angle to the range [0, 2π]
float Radar::normalizeAngle(float angle) {
    while (angle < 0) angle += 2 * M_PI;
    while (angle >= 2 * M_PI) angle -= 2 * M_PI;
    return angle;
}


void Radar::update() {
}