# Aircraft Simulation Project

This project simulates the behavior of aircraft in a 2D space, utilizing SDL for graphical rendering and Python for behavior scripting via `pybind11`. The simulation consists of aircraft that move across the screen in a linear pattern, with updates driven by Python-controlled behavior. The aircraft are rendered and controlled in real-time within an SDL window, and their positions are updated every frame.

## Project Overview

### Key Components:
- **Aircraft Simulation**: 
   - Managing aircraft objects
   - Rendering the graphical window
   - Updating the positions of the aircrafts (with python script)
- **Python Integration**: 
   - The simulation interacts with Python to control the behavior of aircraft via an external Python script 
   - This script defines behavior such as moving aircraft in a linear pattern 
   - Making decisions based on their current positions
- **SDL**: 
   - SDL (Simple DirectMedia Layer) is used for rendering the simulation's graphics and handling window events

## Features

- **Python-Controlled Behavior**: The aircraft's movement is managed by Python scripts via `pybind11`, allowing easy modification and extension of aircraft behavior.

## Screenshot

Here is a screenshot of the simulation's render screen:

#### Screenshot SIM UI
<img src="./assets/images/Aircraft_Radars.png" alt="Render Screen" style="width:100%; max-width:900px; height:auto;">

<video width="600" controls>
  <source src="video.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

## Setup

### Prerequisites & Installing Dependencies
--------------------------------
| Component           | Description                                                                                                                                                 | Notes                                                                                                   |
|---------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------|
| **C++ Compiler**    | Ensure you have a C++ compiler that supports **C++11** or later.                                                                                           | Popular choices: GCC, Clang, or MSVC.                                                                  |
| **SDL2**       | The project depends on **SDL2** for rendering graphics.                                                                                                      | On Windows, you can download SDL2 ```SDL2-devel-2.30.10-VC.zip``` from the [SDL github link](https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-VC.zip)                                   |
| **Python**     | Python (preferably version **3.12**) is required for running behavior scripts.                                                                               | Ensure it is installed and accessible in your system's PATH.                                           |
| **pybind11**   | Used for binding C++ and Python, allowing interaction between the simulation and the Python script.                                                          | Install it via `pip install pybind11` or include it in your C++ project.                               |
| **INIReader**  | The project uses **INIReader** to parse configuration files.                                                                                                | Download it from the [INIReader GitHub repository](https://github.com/benhoyt/inih) and include it.    |
----------------------------------------


### Using `setup.py`
- Setup.py file is present in project
- With `setup.py`, you can build and install the project using the following commands:

1. **Install the project**:
   Navigate to the project where setup.py is present and run:

   ```bash
   python setup.py build_ext --inplace
   ```

### **Run**
- Application in VS2022 and set configuration to ```release 64x``` 