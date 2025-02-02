#include "Simulation.h"
#include <iostream>
#include <stdexcept>
#include <atomic> // For thread-safe running state



// Initialize the static instance to nullptr
std::once_flag flag;
std::unique_ptr<Simulation> Simulation::instance = nullptr;

Simulation::Simulation()
    : coordSystem(-90.0f, 90.0f, -180.0f, 180.0f, 1067, 600), running(false) { // Initialize running to false

    initialize_deploy();

    //################################ python ################################

    // Pass the simulation object to Python
    py::object py_sim = py::cast(this);
    behavior_module.attr("set_simulation")(py_sim);

    //################################ python ################################
}

Simulation::~Simulation() {

}

CoordinateSystem Simulation::getCoordinateSystem() {
    return coordSystem;
}

void Simulation::setDeployMode(SimulationObjectType dm) {
    deployMode = dm;
}

SimulationObjectType Simulation::getDeployMode() {
    return deployMode;
}

std::string Simulation::getSelectedAircraft() {
    return selectedAircraft;
}

std::string Simulation::getSelectedWaypoint() {
    return selectedWaypoint;
}

// Check if the simulation is running
bool Simulation::is_running() const {
    return running;
}

void Simulation::set_running(bool state) {
    running = state;
}

void Simulation::onButtonclick(std::string color) {
    if (color == "red") {
        selectedAircraft = "Red";
        deployMode = SimulationObjectType::Aircraft;
    }

    if (color == "blue") {
        selectedAircraft = "Blue";
        deployMode = SimulationObjectType::Aircraft;
    }

    if (color == "red-waypoint") {
        selectedWaypoint = "Red";
        deployMode = SimulationObjectType::Waypoint;
    }

    if (color == "blue-waypoint") {
        selectedWaypoint = "Blue";
        deployMode = SimulationObjectType::Waypoint;
    }
}

Simulation& Simulation::get_instance() {
    std::call_once(flag, []() {
        instance = std::unique_ptr<Simulation>(new Simulation());
        });
    return *instance;
}

void Simulation::render_single_aircraft(std::string color) {
    if (color == "red") {
        add_aircraft("Fighter - Red", "Red", 100, 60.f, -120.0f, 90.0f, 0.25f, coordSystem);
    }

    if (color == "blue") {
        add_aircraft("Fihgter - Blue", "Blue", 100, -60.f, 120.0f, 270.0f, 0.25f, coordSystem);
    }
}

void Simulation::render_waypoint(std::string color, int x, int y) {
    if (color == "Red") {
        add_waypoint("Fighter - Red", "Red", x, y, coordSystem);
    }
    else if (color == "Blue") {
        add_waypoint("Fighter - Blue", "Blue", x, y, coordSystem);
    }
}

void Simulation::render_single_aircraft(std::string color, int x, int y, float angle) {
    if (color == "Red") {
        add_aircraft("Fighter - Red", "Red", 100, x, y, angle, 0.25f, coordSystem);
    }
    else if (color == "Blue") {
        add_aircraft("Fighter - Blue", "Blue", 100, x, y, angle, 0.25f, coordSystem);
    }
}

void Simulation::render_aircrafts(std::string color) {
    int grid_size = 5; // 7x7 grid
    float spacing = 15.0f; // Distance between aircraft in the grid

    if (color == "red") {
        // Red team: Top-left corner
        float red_start_lat = 90.0f - 20;  // Top of the map
        float red_start_lon = -180.0f + 30; // Left of the map

        // Render Red team
        for (int row = 0; row < grid_size; ++row) {
            for (int col = 0; col < grid_size; ++col) {
                float lat = red_start_lat - row * spacing; // Move downward
                float lon = red_start_lon + col * spacing; // Move rightward
                std::string name = "RedAircraft" + std::to_string(row * grid_size + col + 1);
                add_aircraft(name, "Red", 100, lat, lon, 90.0f, 0.25f, coordSystem);
            }
        }
    }
    
    if (color == "blue") {
        // Blue team: Bottom-right corner
        float blue_start_lat = -90.0f + 20; // Bottom of the map
        float blue_start_lon = 180.0f - 30; // Right of the map

        // Render Blue team
        for (int row = 0; row < grid_size; ++row) {
            for (int col = 0; col < grid_size; ++col) {
                float lat = blue_start_lat + row * spacing; // Move upward
                float lon = blue_start_lon - col * spacing; // Move leftward
                std::string name = "BlueAircraft" + std::to_string(row * grid_size + col + 1);
                add_aircraft(name, "Blue", 100, lat, lon, 270.0f, 0.25f, coordSystem);
            }
        }
    }
    
}

// Add an aircraft
void Simulation::add_aircraft(const std::string& name, const std::string& force, int health, float x, float y, float heading, float speed, CoordinateSystem& coordSystem) {
    // Create an aircraft
    aircrafts.emplace_back(name, force, health, x, y, heading, speed, coordSystem);
}

void Simulation::add_waypoint(const std::string& name, const std::string& force, float x, float y, CoordinateSystem& coordSystem) {
    // Create an aircraft
    waypoints.emplace_back(name, force, x, y, coordSystem);
}

// Get aircrafts (const version)
const std::vector<Aircraft>& Simulation::get_aircrafts() const {
    return aircrafts;
}

const std::vector<Waypoint>& Simulation::get_waypoints() const {
    return waypoints;
}

// Get aircrafts (modifiable version)
std::vector<Aircraft>& Simulation::get_aircrafts_mutable() {
    return aircrafts;
}

// simulation Update
void Simulation::simulation_update() {
    for (auto& aircraft : aircrafts) {
        aircraft.update(); // Update each aircraft's state
    }

    for (auto& waypoint : waypoints) {
        waypoint.update(); // Update each aircraft's state
    }
    //initialize();
    //PyGILState_STATE gstate;
    //gstate = PyGILState_Ensure();

    // Perform Python actions here.
     //behavior_module.attr("sim_update")();
    // behavior_module.attr("call_once")();

    // Release the thread. No Python API allowed beyond this point.
    //PyGILState_Release(gstate);
}

// Assuming necessary headers are included and Simulation, Aircraft, Waypoint classes exist

void Simulation::processSimulation() {
    // Use pointers to original waypoints
    std::deque<Waypoint*> red_waypoints;
    std::deque<Waypoint*> blue_waypoints;

    // Iterate through original waypoints by reference
    for (Waypoint& wp : waypoints) { // Assuming waypoints is std::vector<Waypoint>
        if (wp.get_force() == "Red") {
            red_waypoints.push_back(&wp);
        }
        else if (wp.get_force() == "Blue") {
            blue_waypoints.push_back(&wp);
        }
    }

    // Process each aircraft by reference to modify originals
    for (Aircraft& aircraft : aircrafts) { // Assuming aircrafts is std::vector<Aircraft>
        // Get current position
        double aircraft_lat, aircraft_lon;
        aircraft_lat = aircraft.get_position().first;
        aircraft_lon = aircraft.get_position().second;

        Waypoint* target_waypoint = nullptr;
        std::string force = aircraft.get_force();

        // Assign appropriate waypoint based on force
        if (force == "Red") {
            if (!red_waypoints.empty()) {
                target_waypoint = red_waypoints.front();
                red_waypoints.pop_front();
            }
            else {
                std::cout << "No red waypoints available for aircraft "
                    << aircraft.get_name() << std::endl;
            }
        }
        else if (force == "Blue") {
            if (!blue_waypoints.empty()) {
                target_waypoint = blue_waypoints.front();
                blue_waypoints.pop_front();
            }
            else {
                std::cout << "No blue waypoints available for aircraft "
                    << aircraft.get_name() << std::endl;
            }
        }

        // Move aircraft if valid waypoint exists
        if (target_waypoint) {
            double target_lat, target_lon;
            target_lat = target_waypoint->get_position().first;
            target_lon = target_waypoint->get_position().second;
            aircraft.move_to(target_lat, target_lon);

            std::cout << "Moving Aircraft " << aircraft.get_name()
                << " from (" << aircraft_lat << ", " << aircraft_lon
                << ") to (" << target_lat << ", " << target_lon
                << ") at waypoint " << target_waypoint->get_name()
                << std::endl;
        }
        else {
            std::cout << "Aircraft " << aircraft.get_name()
                << " has no valid waypoint to move to." << std::endl;
        }
    }
}

void Simulation::initialize() {

    processSimulation();

    //PyGILState_STATE gstate;
    //gstate = PyGILState_Ensure();

    //try {
    //    // Call the Python function and capture the return value
    //    pybind11::object result = behavior_module.attr("call_once")();

    //    // Process the returned value (example for a dictionary)
    //    if (pybind11::isinstance<pybind11::dict>(result)) {
    //        pybind11::dict result_dict = result.cast<pybind11::dict>();
    //        std::string status = pybind11::str(result_dict["status"]);
    //        pybind11::list data = result_dict["data"].cast<pybind11::list>();

    //        std::cout << "Status: " << status << "\n";
    //        std::cout << "Data: ";
    //        for (auto item : data) {
    //            std::cout << item.cast<int>() << " ";
    //        }
    //        std::cout << "\n";
    //    }
    //    else {
    //        std::cerr << "Unexpected return type from call_once\n";
    //    }
    //}
    //catch (const pybind11::error_already_set& e) {
    //    std::cerr << "Python error: " << e.what() << "\n";
    //}

    //PyGILState_Release(gstate);

}


