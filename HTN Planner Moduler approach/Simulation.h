#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>
#include <string>
#include "Aircraft.h"
#include "Waypoint.h"
#include <atomic>
#include "CoordinateSystem.h"
#include <iostream>
#include "Button.h"
#include "enums.h"
#include "deque"


#include <pybind11/embed.h>

namespace py = pybind11;




class Simulation {
public:
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    static Simulation& get_instance();

    void render_aircrafts(std::string color);
    void add_aircraft(const std::string& name, const std::string& force, int health, float x, float y, float heading, float speed, CoordinateSystem& coordSystem);
    const std::vector<Aircraft>& get_aircrafts() const;
    const std::vector<Waypoint>& get_waypoints() const;
    std::vector<Aircraft>& get_aircrafts_mutable();
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
    std::string getSelectedAircraft();
    std::string getSelectedWaypoint();

private:
    Simulation();

    std::string selectedAircraft;
    std::string selectedWaypoint;

    std::vector<Aircraft> aircrafts;
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
    // Import Python script
    py::module behavior_module = py::module::import("aircraft_behavior");

    //################################ python ################################
};   

#endif
