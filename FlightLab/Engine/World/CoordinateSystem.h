#pragma once
#include <utility> // For std::pair

class CoordinateSystem {
private:
    float min_latitude;   // Minimum latitude value (e.g., 0.0)
    float max_latitude;   // Maximum latitude value (e.g., 100.0)
    float min_longitude;  // Minimum longitude value (e.g., 0.0)
    float max_longitude;  // Maximum longitude value (e.g., 100.0)
    int screen_width;     // Screen width in pixels
    int screen_height;    // Screen height in pixels

public:
    // Constructor
	CoordinateSystem() = default;
    CoordinateSystem(float min_lat, float max_lat, float min_lon, float max_lon,
        int screen_w, int screen_h);

    // Convert latitude/longitude to screen coordinates
    std::pair<int, int> to_screen_coordinates(float latitude, float longitude) const;

    // Convert screen coordinates to latitude/longitude
    std::pair<float, float> to_lat_lon(int screen_x, int screen_y) const;

    // Wrap latitude and longitude to stay within bounds
    void wrap_coordinates(float& latitude, float& longitude) const;

    // Getters
    float get_min_latitude() const;
    float get_max_latitude() const;
    float get_min_longitude() const;
    float get_max_longitude() const;
};
