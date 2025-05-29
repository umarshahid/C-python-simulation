#define SDL_MAIN_HANDLED
#include "Randerer/RenderManager.h"
#include <iostream>


//int Aircraft::nextID = 1;

int main(int argc, char* argv[]) {
    _putenv("PROJ_LIB=D:\\repos\\FlightLab\\3rd-party\\vcpkg\\installed\\x64-windows\\share\\proj");


        
    try {
        RenderManager& renderManager = RenderManager::get_instance();
		renderManager.run();
    }
    catch (const std::exception& e) {
        std::cerr << "C++ Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
