#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Button.h"
#include "Simulation.h"
#include "utils.cpp"
#include "Aircraft.h"
#include "FileLoader.cpp"
#include "string"

class RenderManager
{
	RenderManager();
	static std::unique_ptr<RenderManager> instance; // Use unique_ptr for automatic memory management

	static SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
	void applyColorMod(SDL_Texture* texture, std::string force) const;

	std::string iconPathAircraft;
	std::string iconPathWaypoint;


	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<Button> buttons;
	bool quit;
	float angle = 0.0f;

public:
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;
	static RenderManager& get_instance(); // Returns a reference to the singleton

	~RenderManager();
	void run();
	void handleMouseEvent(const SDL_Event& e);
	void handleMouseClick(int x, int y);
	void handleMouseWheel(SDL_Event& e);
	void render_aircraft_preview(const std::string& force, int x, int y);
	void drawAircraft(Aircraft* aircraft) const;
	void drawWaypoint(Waypoint* waypoint) const;
	void drawRadarCone(Radar* radar, int centerX, int centerY, float heading) const;
};

