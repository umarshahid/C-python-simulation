#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "RenderManager.h"
#include <iostream>

int Aircraft::nextID = 1;

int main(int argc, char* argv[]) {
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
