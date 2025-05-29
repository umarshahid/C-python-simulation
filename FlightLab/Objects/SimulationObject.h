#pragma once
#include <string>
#include "../Engine/World/CoordinateSystem.h"

//enum class SimulationObjectType { Aircraft, Waypoint, Missile, Unknown };

class SimulationObject {
//protected:
//    std::string id;
//    SimulationObjectType type;
//
//    std::string name;
//    std::string force;
//    
//    float latitude, longitude;
//    float orientation;
//
//    SDL_Texture* texture;
//    CoordinateSystem& coordinateSystem;
//
//public:
//    SimulationObject(const std::string& id, SimulationObjectType type, const std::string& name, 
//        const std::string& force, float startLatitude, float startLongitude, float orientation, CoordinateSystem& coordinateSystem)
//        : id(id), type(type), name(name), force(force), latitude(startLatitude), longitude(startLongitude), 
//        orientation(orientation),  coordinateSystem(coordinateSystem), texture(nullptr) {
//    }
//
//    virtual ~SimulationObject() = default;
//
//    virtual void update(float deltaTime) = 0;
//    virtual void render(SDL_Renderer* renderer) const = 0;
//
//    std::string getId() const { return id; }
//    SimulationObjectType getType() const { return type; }
//    std::pair<float, float> get_position() const { return { latitude, longitude }; }
};


