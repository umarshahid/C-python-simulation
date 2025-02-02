import aircraft_simulation
import time
import random
import math

# Global variable to store the simulation object
simulation = None

def set_simulation(sim):
    global simulation
    simulation = sim
    print("Simulation object set in Python!")

def call_once():
    print("call_once() is called...")

    if simulation is None:
        print("Simulation is not set!")
        return

    aircrafts = simulation.get_aircrafts()
    waypoints = simulation.get_waypoints()
   
    # Separate waypoints by color
    red_waypoints = [wp for wp in waypoints if wp.get_force() == "Red"]
    blue_waypoints = [wp for wp in waypoints if wp.get_force() == "Blue"]

    # Iterate through each aircraft and move it to a waypoint
    for aircraft in aircrafts:
        # Get the aircraft's current position (latitude, longitude)
        aircraft_lat, aircraft_lon = aircraft.get_position()

        # Initialize the target waypoint
        target_waypoint = None
        
        if aircraft.get_force() == "Red":
            # Assign a red waypoint to the red aircraft
            if red_waypoints:
                target_waypoint = red_waypoints.pop(0)  # Get the first available red waypoint
            else:
                print("No red waypoints available for aircraft", aircraft.get_name())
        elif aircraft.get_force() == "Blue":
            # Assign a blue waypoint to the blue aircraft
            if blue_waypoints:
                target_waypoint = blue_waypoints.pop(0)  # Get the first available blue waypoint
            else:
                print("No blue waypoints available for aircraft", aircraft.get_name())
        
        # If we have a valid target waypoint, move the aircraft
        if target_waypoint:
            # Get the target waypoint's position (latitude, longitude)
            target_lat, target_lon = target_waypoint.get_position()
            
            # Move the aircraft to the target waypoint's latitude and longitude
            aircraft.move_to(target_lat, target_lon)  
            aircraft_name = aircraft.get_name()  # Get the name of the aircraft
            print(f"Moving Aircraft {aircraft_name} from ({aircraft_lat}, {aircraft_lon}) to ({target_lat}, {target_lon}) at waypoint {target_waypoint.get_name()}")
        else:
            print(f"Aircraft {aircraft.get_name()} has no valid waypoint to move to.")



    return {"status": "success", "data": [1,2,3]}

\
def sim_update():
    if simulation is None:
        print("Simulation is not set!")
        return

    # Perform a single update for aircrafts
#    aircrafts = simulation.get_aircrafts()
#
#    target_locations = [
#        [(70, -120), (-70, 120)],
#        [(-70, 120), (70, -120)],
#        [(70, -120), (-70, 120)],
#        [(-70, 120), (70, -120)]
#    ]
#    # Check if 10 seconds have passed
#    global last_update_time, i
#    current_time = time.time()
#    
#    if (current_time - last_update_time >= 10 and i < len(target_locations)):
#        print("move to next location: ", target_locations[i])
#        for aircraft, target in zip(aircrafts, target_locations[i]):
#            aircraft_name = aircraft.get_name()
#            lat, lon = target
#            aircraft.move_to(lat, lon)
#            print(f"Moving Aircraft {aircraft_name} to ({lat}, {lon}) in lat/lon")
#    
#        # Reset the last update time
#        last_update_time = current_time
#        i+=1;