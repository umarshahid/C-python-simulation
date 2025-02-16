#include "RenderManager.h"

// Initialize the static instance to nullptr
std::once_flag flag1;
std::unique_ptr<RenderManager> RenderManager::instance = nullptr;

RenderManager& RenderManager::get_instance() {
    std::call_once(flag1, []() {
        instance = std::unique_ptr<RenderManager>(new RenderManager());
    });
    return *instance;
}

RenderManager::RenderManager() : window(nullptr), renderer(nullptr), quit(false) {

    iconPathAircraft = FileLoader::getSimulationObjectTexture(SimulationObjectType::Aircraft);
    iconPathWaypoint = FileLoader::getSimulationObjectTexture(SimulationObjectType::Waypoint);
    iconPathMissile = FileLoader::getSimulationObjectTexture(SimulationObjectType::Missile); 

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
    }

    window = SDL_CreateWindow("Aircraft Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1067, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
    }

    buttons.push_back({ {1030, 10, 25, 25}, "Red", SimulationObjectType::Aircraft, [this]() {
        Simulation::get_instance().onButtonclick("red");
    } });
    buttons.push_back({ {1030, 40, 25, 25}, "Blue", SimulationObjectType::Aircraft, [this]() {
        Simulation::get_instance().onButtonclick("blue");
    } });

    buttons.push_back({ {1030, 70, 25, 25}, "Red", SimulationObjectType::Waypoint, [this]() {
        Simulation::get_instance().onButtonclick("red-waypoint");
    } });
    buttons.push_back({ {1030, 100, 25, 25}, "Blue", SimulationObjectType::Waypoint, [this]() {
        Simulation::get_instance().onButtonclick("blue-waypoint");
    } });

    buttons.push_back({ {1030, 550, 25, 25}, "Green", SimulationObjectType::Unknown, [this]() { Simulation::get_instance().initialize(); } });

}

RenderManager::~RenderManager() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void RenderManager::run() {
    SDL_Event e;
    Simulation::get_instance().set_running(true); // Set running to true when the simulation starts

    int mouseX = 0, mouseY = 0;

    while (!quit) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                //handleMouseClick(e.button.x, e.button.y);
                handleMouseEvent(e);
            }
            else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                handleMouseWheel(e);
            }

        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //black
        //SDL_SetRenderDrawColor(renderer, 54, 69, 79, 255); //Charcol
        //SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255); //gray
        //SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255); //shadow
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //white
        SDL_RenderClear(renderer);

        // Render buttons
        for (const auto& button : buttons) {
            button.render(renderer);
        }


        // Render aircraft following mouse pointer in deployMode
        if (Simulation::get_instance().getDeployMode() == SimulationObjectType::Aircraft && !Simulation::get_instance().getSelectedAircraft().empty()) {
            render_aircraft_preview(Simulation::get_instance().getSelectedAircraft(), mouseX, mouseY);
        }
        // Simulation Update Call
        Simulation::get_instance().simulation_update();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // Cap frame rate to ~60 FPS
    }
    Simulation::get_instance().set_running(false); // Set running to false when the simulation stops
}

void RenderManager::handleMouseEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;

        std::cout << "Left click detected at (" << x << ", " << y << ")\n";

        bool buttonClicked = false;

        // Check if any button is clicked
        for (const auto& button : buttons) {
            if (button.isClicked(x, y)) {
                button.onClick();
                buttonClicked = true;  // Mark that a button was clicked
                std::cout << "Button clicked at (" << x << ", " << y << ")\n";
                break;  // Exit loop since a button was clicked
            }
        }

        // If no button is clicked, check for deploying aircraft
        if (!buttonClicked) {
            std::cout << "Screen clicked at (" << x << ", " << y << ")\n";
            std::cout << "Deploy mode: " << simulationObjectTypeToString(Simulation::get_instance().getDeployMode())
                << ", Selected aircraft: " << Simulation::get_instance().getSelectedAircraft() << "\n";

            if (Simulation::get_instance().getDeployMode() == SimulationObjectType::Aircraft && !Simulation::get_instance().getSelectedAircraft().empty()) {
                // Deploy the selected aircraft at the mouse location
                std::pair<int, int> lat_lon = Simulation::get_instance().getCoordinateSystem().to_lat_lon(x, y);
                int lat = lat_lon.first;
                int lon = lat_lon.second;
                Simulation::get_instance().render_single_aircraft(Simulation::get_instance().getSelectedAircraft(), lat, lon, angle);
            }

            if (Simulation::get_instance().getDeployMode() == SimulationObjectType::Waypoint && !Simulation::get_instance().getSelectedWaypoint().empty()) {
                // Deploy the selected aircraft at the mouse location
                std::pair<int, int> lat_lon = Simulation::get_instance().getCoordinateSystem().to_lat_lon(x, y);
                int lat = lat_lon.first;
                int lon = lat_lon.second;
                Simulation::get_instance().render_waypoint(Simulation::get_instance().getSelectedWaypoint(), lat, lon);
            }
        }

    }
    else {
        // Always reset deploy mode after handling the click
        if (Simulation::get_instance().getDeployMode() != SimulationObjectType::Unknown) {
            std::cout << "Deploy mode deactivated.\n";
            Simulation::get_instance().setDeployMode(SimulationObjectType::Unknown);
        }
    }
}


void RenderManager::handleMouseClick(int x, int y) {
    bool buttonClicked = false;

    // Check if any button is clicked
    for (const auto& button : buttons) {
        if (button.isClicked(x, y)) {
            button.onClick();
            buttonClicked = true;  // Mark that a button was clicked
            std::cout << "Button clicked at (" << x << ", " << y << ")\n";
            break;  // Exit loop since a button was clicked
        }
    }

    // If no button is clicked, check for deploying aircraft
    if (!buttonClicked) {
        std::cout << "Screen clicked at (" << x << ", " << y << ")\n";
        std::cout << "Deploy mode: " << simulationObjectTypeToString(Simulation::get_instance().getDeployMode())
            << ", Selected aircraft: " << Simulation::get_instance().getSelectedAircraft() << "\n";

        if (Simulation::get_instance().getDeployMode() == SimulationObjectType::Aircraft && !Simulation::get_instance().getSelectedAircraft().empty()) {
            // Deploy the selected aircraft at the mouse location
            std::pair<int, int> lat_lon = Simulation::get_instance().getCoordinateSystem().to_lat_lon(x, y);
            int lat = lat_lon.first;
            int lon = lat_lon.second;
            Simulation::get_instance().render_single_aircraft(Simulation::get_instance().getSelectedAircraft(), lat, lon, angle);
            //deployMode = false;  // Turn off deploy mode after deploying
        }
    }
}

void RenderManager::handleMouseWheel(SDL_Event& e) {
    if (e.type == SDL_MOUSEWHEEL) {
        angle += e.wheel.y * 5.0f; // Adjust rotation by 5 degrees per scroll step
    }
}

void RenderManager::render_aircraft_preview(const std::string& force, int x, int y) {

    SDL_Texture* aircraftTexture = loadTexture(renderer, iconPathAircraft);
    if (!aircraftTexture) return;

    int texture_width = 32, texture_height = 32;
    SDL_QueryTexture(aircraftTexture, nullptr, nullptr, &texture_width, &texture_height);

    SDL_Rect renderQuad = { x - texture_width / 2, y - texture_height / 2, texture_width, texture_height };

    // Set aircraft color (based on the force)
    applyColorMod(aircraftTexture, force);

    SDL_RenderCopyEx(renderer, aircraftTexture, nullptr, &renderQuad, angle, nullptr, SDL_FLIP_NONE);
    SDL_DestroyTexture(aircraftTexture);

}

// ******************* Aircraft Drawing *******************
void RenderManager::drawAircraft(Aircraft* aircraft) const {

    int screen_x = aircraft->get_position3().x;
    int screen_y = aircraft->get_position3().y;

    drawRadarCone(aircraft->getRadar(), screen_x, screen_y, aircraft->get_heading() - 90);

    SDL_Texture* aircraftTexture = loadTexture(renderer, iconPathAircraft);
    if (!aircraftTexture) return;

    // Calculate the render rectangle for the image
    // Get the dimensions of the aircraft texture
    int texture_width = 32, texture_height = 32;
    SDL_QueryTexture(aircraftTexture, nullptr, nullptr, &texture_width, &texture_height);

    // Calculate the position for the aircraft image (centered around the coordinates)
    SDL_Rect renderQuad = { screen_x - texture_width / 2, screen_y - texture_height / 2, texture_width, texture_height };

    // Set aircraft color (based on the force)
    applyColorMod(aircraftTexture, aircraft->get_force());

    // Rotate the aircraft image based on its heading (rotate around its center)
    SDL_RenderCopyEx(renderer, aircraftTexture, nullptr, &renderQuad, aircraft->get_heading(), nullptr, SDL_FLIP_NONE);

    // Free the texture after rendering
    SDL_DestroyTexture(aircraftTexture);

    if (aircraft->get_isMoving()) {

        int target_x = aircraft->get_target_position_xy().first;
        int target_y = aircraft->get_target_position_xy().second;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green for target line
        SDL_RenderDrawLine(renderer, screen_x, screen_y, target_x, target_y);
    }

}

void RenderManager::lockLine(Vector3* target, Vector3* self, std::string force) {
    applyLineColor(force);
	SDL_RenderDrawLine(renderer, self->x, self->y, target->x, target->y);
}

// ******************* Waypoint Drawing *******************
void RenderManager::drawWaypoint(Waypoint* waypoint) const {

    // Get screen coordinates of the waypoint
    int screen_x = waypoint->get_position_xy().first;
    int screen_y = waypoint->get_position_xy().second;

    SDL_Texture* waypointTexture = loadTexture(renderer, iconPathWaypoint);
    if (!waypointTexture) return;

    int texture_width = 32, texture_height = 32;
    SDL_QueryTexture(waypointTexture, nullptr, nullptr, &texture_width, &texture_height);

    // Calculate the position for the waypoint image (centered around the coordinates)
    SDL_Rect renderQuad = { screen_x - texture_width / 2, screen_y - texture_height / 2, texture_width, texture_height };

    // Set waypoint color (based on the force)
    applyColorMod(waypointTexture, waypoint->get_force());

	// Waypoints do not rotate; passing zero as angle
    SDL_RenderCopyEx(renderer, waypointTexture, nullptr, &renderQuad, 0, nullptr, SDL_FLIP_NONE);
    SDL_DestroyTexture(waypointTexture);

}



void RenderManager::drawRadarCone(Radar* radar, int centerX, int centerY, float heading) const {


    // Convert heading and angles to radians
    float headingRad = heading * M_PI / 180.0f;
    float leftEdgeRad = headingRad - (radar->getRadarAngle() * M_PI / 180.0f);
    float rightEdgeRad = headingRad + (radar->getRadarAngle() * M_PI / 180.0f);

    // Calculate the end points of the cone edges
    int leftX = centerX + radar->getRadarRadius() * cos(leftEdgeRad);
    int leftY = centerY + radar->getRadarRadius() * sin(leftEdgeRad);
    int rightX = centerX + radar->getRadarRadius() * cos(rightEdgeRad);
    int rightY = centerY + radar->getRadarRadius() * sin(rightEdgeRad);

    // Set radar boundary color (yellow)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    // Draw the cone's boundary edges
    SDL_RenderDrawLine(renderer, centerX, centerY, leftX, leftY); // Left edge
    SDL_RenderDrawLine(renderer, centerX, centerY, rightX, rightY); // Right edge

    // Optionally, draw the arc connecting the edges to complete the cone boundary
    const int segments = 50; // Number of segments for smoothness
    for (int i = 0; i < segments; ++i) {
        float t1 = leftEdgeRad + (i / static_cast<float>(segments)) * (rightEdgeRad - leftEdgeRad);
        float t2 = leftEdgeRad + ((i + 1) / static_cast<float>(segments)) * (rightEdgeRad - leftEdgeRad);
        int arcX1 = centerX + radar->getRadarRadius() * cos(t1);
        int arcY1 = centerY + radar->getRadarRadius() * sin(t1);
        int arcX2 = centerX + radar->getRadarRadius() * cos(t2);
        int arcY2 = centerY + radar->getRadarRadius() * sin(t2);
        SDL_RenderDrawLine(renderer, arcX1, arcY1, arcX2, arcY2);
    }
}

SDL_Surface* RenderManager::ResizeSurface(SDL_Surface* source, int newWidth, int newHeight) {
    SDL_Surface* resized = SDL_CreateRGBSurface(0, newWidth, newHeight, 32,
        source->format->Rmask, source->format->Gmask, source->format->Bmask, source->format->Amask);

    if (!resized) {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Rect srcRect = { 0, 0, source->w, source->h };
    SDL_Rect dstRect = { 0, 0, newWidth, newHeight };

    SDL_BlitScaled(source, &srcRect, resized, &dstRect);
    return resized;
}

void RenderManager::drawMissile(Missile* missile) const {

    int screen_x = missile->get_position3().x;
    int screen_y = missile->get_position3().y;

    SDL_Texture* missileTexture = loadTexture(renderer, iconPathMissile);
    if (!missileTexture) return;

    // Calculate the render rectangle for the image
    // Get the dimensions of the aircraft texture
    int texture_width = 32, texture_height = 32;
    SDL_QueryTexture(missileTexture, nullptr, nullptr, &texture_width, &texture_height);

    // Calculate the position for the aircraft image (centered around the coordinates)
    SDL_Rect renderQuad = { screen_x - texture_width / 2, screen_y - texture_height / 2, texture_width, texture_height };

    // Set aircraft color (based on the force)
    applyColorMod(missileTexture, missile->force);

    // Rotate the aircraft image based on its heading (rotate around its center)
    SDL_RenderCopyEx(renderer, missileTexture, nullptr, &renderQuad, missile->get_heading(), nullptr, SDL_FLIP_NONE);

    // Free the texture after rendering
    SDL_DestroyTexture(missileTexture);

    if (missile->active) {

        int target_x = missile->_targetPos.x;
        int target_y = missile->_targetPos.y;

        //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green for target line
        applyLineColor(missile->force);
        SDL_RenderDrawLine(renderer, screen_x, screen_y, target_x, target_y);
    }

}

// common for all render manager

SDL_Texture* RenderManager::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Error loading texture (" << path << "): " << SDL_GetError() << "\n";
    }
    return texture;
}

void RenderManager::applyColorMod(SDL_Texture* texture, std::string force) const {
    if (force == "Blue") {
        SDL_SetTextureColorMod(texture, 30, 144, 255); // Blue
    }
    else if (force == "Red") {
        SDL_SetTextureColorMod(texture, 220, 20, 60);  // Red
    }
    else if (force == "Green") {
        SDL_SetTextureColorMod(texture, 80, 200, 120); // Green
    }
    else {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }
}

void RenderManager::applyLineColor(std::string force) const {
    if (force == "Blue") {
        SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255); // Blue
    }
    else if (force == "Red") {
        SDL_SetRenderDrawColor(renderer, 220, 20, 60, 255);  // Red
    }
    else if (force == "Green") {
        SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255); // Green
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
}