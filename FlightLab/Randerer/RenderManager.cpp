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

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        throw std::runtime_error(std::string("SDL_GetCurrentDisplayMode Error: ") + SDL_GetError());
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;

    // You can set the window size to a percentage of the screen size
    int windowWidth = static_cast<int>(screenWidth * 0.8);  // 80% of screen width
    int windowHeight = static_cast<int>(screenHeight * 0.8); // 80% of screen height

    window = SDL_CreateWindow(
        "Aircraft Simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    std::cout <<"Window Width: "<< windowWidth << ", Window Height: " << windowHeight << "\n";

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


	Simulation::get_instance().setCoordinateSystem(-90.0f, 90.0f, -180.0f, 180.0f, windowWidth, windowHeight);

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

void RenderManager::onWindowResized(int newWidth, int newHeight) {
    int buttonSpacing = 3;
    int buttonWidth = 32;
    int buttonHeight = 32;

    int startX = newWidth - buttonWidth - 10; // 10px padding from the right edge
    int startY = 10; // Start from the top with 10px padding

    for (size_t i = 0; i < buttons.size(); ++i) {
        SDL_Rect newRect = { startX, startY + static_cast<int>(i) * (buttonHeight + buttonSpacing), buttonWidth, buttonHeight };
        buttons[i].setRect(newRect);
    }
}


RenderManager::~RenderManager() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}


void RenderManager::run() {
    SDL_Event e;
    Simulation::get_instance().set_running(true);
    int newWidth, newHeight;
    SDL_GetWindowSize(window, &newWidth, &newHeight);

    int mouseX = 0, mouseY = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleMouseEvent(e);
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mouseX, &mouseY);
                break;
            case SDL_MOUSEWHEEL:
                handleMouseWheel(e);
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    newWidth = e.window.data1;
                    newHeight = e.window.data2;
                    onWindowResized(newWidth, newHeight);
                }
                break;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // Define map area: 75% of the window width for the map
        int mapWidth = static_cast<int>(newWidth * 0.75);
        int mapHeight = newHeight;

        // Render shapefile within the map viewport
        std::string shapefilePathStr = FileLoader::getMapFile();
        const char* shapefilePath = shapefilePathStr.c_str();
        RenderShapefile(renderer, shapefilePath, mapWidth, mapHeight);

        // Render buttons on the remaining right side (full window space)
        for (const auto& button : buttons) {
            button.render(renderer);
        }

        // Render aircraft preview (mouse-following)
        if (Simulation::get_instance().getDeployMode() == SimulationObjectType::Aircraft &&
            !Simulation::get_instance().getSelectedAircraft().empty()) {
            render_aircraft_preview(Simulation::get_instance().getSelectedAircraft(), mouseX, mouseY);
        }

        // Simulation update
        Simulation::get_instance().simulation_update();

        SDL_RenderPresent(renderer);
        SDL_Delay(5);
        //SDL_Delay(16);
    }

    Simulation::get_instance().set_running(false);
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

    drawRadarCone(aircraft->getRadar(), screen_x, screen_y, aircraft->get_heading() - 90, aircraft->get_force());

    SDL_Texture* aircraftTexture = loadTexture(renderer, iconPathAircraft);
    if (!aircraftTexture) return;

    // Calculate the render rectangle for the image
    // Get the dimensions of the aircraft texture
    int texture_width = 32, texture_height = 32;
    SDL_QueryTexture(aircraftTexture, nullptr, nullptr, &texture_width, &texture_height);

    // Calculate the position for the aircraft image (centered around the coordinates)
    //SDL_Rect renderQuad = { screen_x - texture_width / 2, screen_y - texture_height / 2, texture_width, texture_height };
    SDL_FRect renderQuad = { screen_x - texture_width / 2, screen_y - texture_height / 2, texture_width, texture_height };

    // Set aircraft color (based on the force)
    applyColorMod(aircraftTexture, aircraft->get_force());

    // Rotate the aircraft image based on its heading (rotate around its center)
    SDL_RenderCopyExF(renderer, aircraftTexture, nullptr, &renderQuad, aircraft->get_heading(), nullptr, SDL_FLIP_NONE);
    //SDL_RenderCopyEx(renderer, aircraftTexture, nullptr, &renderQuad, aircraft->get_heading(), nullptr, SDL_FLIP_NONE);

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



void RenderManager::drawRadarCone(Radar* radar, int centerX, int centerY, float heading, std::string force) const {


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
    //SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    applyLineColor(force);

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

void RenderManager::RenderPoint(SDL_Renderer* renderer, OGRPoint* point, double minX, double minY, double maxX, double maxY, int screenWidth, int screenHeight) {
    int screenX = (int)((point->getX() - minX) / (maxX - minX) * screenWidth);
    int screenY = (int)((maxY - point->getY()) / (maxY - minY) * screenHeight);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, screenX, screenY);
}

void RenderManager::RenderLine(SDL_Renderer* renderer, OGRLineString* line, double minX, double minY, double maxX, double maxY, int screenWidth, int screenHeight) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    for (int i = 0; i < line->getNumPoints() - 1; i++) {
        int x1 = (int)((line->getX(i) - minX) / (maxX - minX) * screenWidth);
        int y1 = (int)((maxY - line->getY(i)) / (maxY - minY) * screenHeight);
        int x2 = (int)((line->getX(i + 1) - minX) / (maxX - minX) * screenWidth);
        int y2 = (int)((maxY - line->getY(i + 1)) / (maxY - minY) * screenHeight);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void RenderManager::RenderPolygon(SDL_Renderer* renderer, OGRPolygon* polygon, double minX, double minY, double maxX, double maxY, int screenWidth, int screenHeight) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    OGRLinearRing* ring = polygon->getExteriorRing();
    for (int i = 0; i < ring->getNumPoints() - 1; i++) {
        int x1 = (int)((ring->getX(i) - minX) / (maxX - minX) * screenWidth);
        int y1 = (int)((maxY - ring->getY(i)) / (maxY - minY) * screenHeight);
        int x2 = (int)((ring->getX(i + 1) - minX) / (maxX - minX) * screenWidth);
        int y2 = (int)((maxY - ring->getY(i + 1)) / (maxY - minY) * screenHeight);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void RenderManager::RenderShapefile(SDL_Renderer* renderer, const char* shapefilePath, int renderWidth, int renderHeight) {
    GDALAllRegister();

    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(shapefilePath, GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (!dataset) {
        std::cerr << "Failed to open shapefile!" << std::endl;
        return;
    }

    OGRLayer* layer = dataset->GetLayer(0);
    OGRFeature* feature;

    OGREnvelope envelope;
    layer->GetExtent(&envelope);

    double minX = envelope.MinX;
    double maxX = envelope.MaxX;
    double minY = envelope.MinY;
    double maxY = envelope.MaxY;

    double shapeWidth = maxX - minX;
    double shapeHeight = maxY - minY;

    double scaleX = renderWidth / shapeWidth;
    double scaleY = renderHeight / shapeHeight;
    double scale = std::min(scaleX, scaleY);

    double shapeMidX = (minX + maxX) / 2.0;
    double shapeMidY = (minY + maxY) / 2.0;

    double screenMidX = renderWidth / 2.0;
    double screenMidY = renderHeight / 2.0;

    auto TransformToPixel = [&](double x, double y) -> SDL_Point {
        int px = static_cast<int>(screenMidX + (x - shapeMidX) * scale);
        int py = static_cast<int>(screenMidY - (y - shapeMidY) * scale); // Y is flipped

#ifdef DEBUG
        if (px < 0 || px >= renderWidth || py < 0 || py >= renderHeight) {
            std::cerr << "Point out of bounds: (" << px << ", " << py << ")" << std::endl;
        }
#endif

        return ClampToScreen(px, py, renderWidth, renderHeight);
    };

    while ((feature = layer->GetNextFeature()) != nullptr) {
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (!geometry) continue;

        OGRwkbGeometryType geomType = wkbFlatten(geometry->getGeometryType());

        if (geomType == wkbPolygon || geomType == wkbMultiPolygon) {
            auto DrawPolygon = [&](OGRPolygon* poly) {
                OGRLinearRing* ring = poly->getExteriorRing();
                int numPoints = ring->getNumPoints();

                if (numPoints < 3) return; // Ignore degenerate polygons

                std::vector<SDL_Point> points(numPoints + 1);

                for (int i = 0; i < numPoints; ++i) {
                    points[i] = TransformToPixel(ring->getX(i), ring->getY(i));
                }
                points[numPoints] = points[0];

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLines(renderer, points.data(), numPoints + 1);
            };

            if (geomType == wkbPolygon) {
                DrawPolygon((OGRPolygon*)geometry);
            }
            else if (geomType == wkbMultiPolygon) {
                OGRMultiPolygon* multiPolygon = (OGRMultiPolygon*)geometry;
                for (int i = 0; i < multiPolygon->getNumGeometries(); ++i) {
                    OGRPolygon* subPolygon = (OGRPolygon*)multiPolygon->getGeometryRef(i);
                    DrawPolygon(subPolygon);
                }
            }
        }

        OGRFeature::DestroyFeature(feature);
    }

    GDALClose(dataset);
}

SDL_Point RenderManager::ClampToScreen(int x, int y, int screenWidth, int screenHeight) {
    SDL_Point p;
    p.x = std::min(std::max(x, 0), screenWidth - 1);
    p.y = std::min(std::max(y, 0), screenHeight - 1);
    return p;
}
