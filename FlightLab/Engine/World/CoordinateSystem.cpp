#include "CoordinateSystem.h"
#include <cmath> // For floor and fmod
#include <iostream>

// Constructor

CoordinateSystem::CoordinateSystem(float minLat, float maxLat, float minLon, float maxLon, int screenWidth, int screenHeight)
    : min_latitude(minLat), max_latitude(maxLat),
    min_longitude(minLon), max_longitude(maxLon),
    screen_width(screenWidth), screen_height(screenHeight) {
    std::cout << "CoordinateSystem initialized with bounds: "
        << "Lat[" << minLat << ", " << maxLat << "], "
        << "Lon[" << minLon << ", " << maxLon << "]\n";
}

std::pair<int, int> CoordinateSystem::to_screen_coordinates(float latitude, float longitude) const {

    // Clamp latitude and longitude
    float clamped_lat = std::max(min_latitude, std::min(latitude, max_latitude));
    float clamped_lon = std::max(min_longitude, std::min(longitude, max_longitude));

    // Normalize latitude and longitude
    float normalized_lat = (clamped_lat - min_latitude) / (max_latitude - min_latitude);
    float normalized_lon = (clamped_lon - min_longitude) / (max_longitude - min_longitude);

    // Invert Y-axis for latitude
    normalized_lat = 1.0f - normalized_lat;

    // Map to screen space
    int screen_x = static_cast<int>(normalized_lon * screen_width);
    int screen_y = static_cast<int>(normalized_lat * screen_height);

    return { screen_x, screen_y };
}


std::pair<float, float> CoordinateSystem::to_lat_lon(int screen_x, int screen_y) const {
    // Normalize screen coordinates to 0-1 range
    float normalized_lon = static_cast<float>(screen_x) / screen_width;
    float normalized_lat = 1.0f - (static_cast<float>(screen_y) / screen_height); // Invert y-axis

    // Convert to latitude and longitude
    float latitude = min_latitude + normalized_lat * (max_latitude - min_latitude);
    float longitude = min_longitude + normalized_lon * (max_longitude - min_longitude);

    return { latitude, longitude };
}

// Wrap latitude and longitude to stay within bounds
void CoordinateSystem::wrap_coordinates(float& latitude, float& longitude) const {
    // Wrap latitude
    if (latitude < min_latitude) {
        latitude = max_latitude - (min_latitude - latitude);
    }
    else if (latitude > max_latitude) {
        latitude = min_latitude + (latitude - max_latitude);
    }

    // Wrap longitude
    if (longitude < min_longitude) {
        longitude = max_longitude - (min_longitude - longitude);
    }
    else if (longitude > max_longitude) {
        longitude = min_longitude + (longitude - max_longitude);
    }
}

// Getters
float CoordinateSystem::get_min_latitude() const {
    return min_latitude;
}

float CoordinateSystem::get_max_latitude() const {
    return max_latitude;
}

float CoordinateSystem::get_min_longitude() const {
    return min_longitude;
}

float CoordinateSystem::get_max_longitude() const {
    return max_longitude;
}
