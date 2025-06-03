#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>
#include <string>
#include "../Objects/Aircraft.h"
#include "../Objects/Waypoint.h"
#include <atomic>
#include "World/CoordinateSystem.h"
#include <iostream>
#include "../Randerer/Button.h"
#include "../Utils/enums.h"
#include "deque"
#include <memory>


#include <pybind11/embed.h>

namespace py = pybind11;




class Simulation {
public:
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    static Simulation& get_instance();

    //double _dt = 1.0 / 60.0; // 50ms time step
    double _dt = 0.016; // 50ms time step
    //double _dt = 0.008; // 50ms time step

    void render_aircrafts(std::string color);
    void add_aircraft(const std::string& name, const std::string& force, int health, float x, float y, float heading, float speed, CoordinateSystem& coordSystem);
    const std::vector<std::unique_ptr<Aircraft>>& get_aircrafts() const;
    const std::vector<Waypoint>& get_waypoints() const;
    //std::vector<Aircraft*>& get_aircrafts_mutable();
	std::vector<std::unique_ptr<Aircraft>>& get_aircrafts_mutable();

    bool is_running() const;
    void set_running(bool state);
    ~Simulation();
    void simulation_update();
    void initialize();

    void render_single_aircraft(std::string color);
    void render_single_aircraft(std::string color, int x, int y, float angle);
    void onButtonclick(std::string color);
    void add_waypoint(const std::string& name, const std::string& force, float x, float y, CoordinateSystem& coordSystem);
    void render_waypoint(std::string color, int x, int y);
    void processSimulation();
    SimulationObjectType getDeployMode();
    void setDeployMode(SimulationObjectType dm);

    CoordinateSystem getCoordinateSystem();
    int setCoordinateSystem(float min_lat, float max_lat, float min_lon, float max_lon,
        int screen_w, int screen_h);
    std::string getSelectedAircraft();
    std::string getSelectedWaypoint();
    void remove_aircraft(Aircraft* target);

private:
    Simulation();

    std::string selectedAircraft;
    std::string selectedWaypoint;

    //std::vector<Aircraft*> aircrafts;
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
    std::vector<Waypoint> waypoints;


    std::atomic<bool> running;
    static std::unique_ptr<Simulation> instance; 
    SimulationObjectType deployMode;
    

    void initialize_deploy() { deployMode = SimulationObjectType::Unknown; selectedAircraft = ""; selectedWaypoint=""; }

    CoordinateSystem coordSystem;

    //################################ python ################################
    bool is_initialized = false;
    // Initializes Python interpreter
    py::scoped_interpreter guard{}; 
    //// Import Python script

    py::module behavior_module = py::module::import("aircraft_behavior");

    //################################ python ################################
};   

#endif
