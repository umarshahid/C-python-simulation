//#include "Button.h"
//
//Button::Button(SDL_Rect rect, const std::string& force, SimulationObjectType buttonType, std::function<void()> onClick, const std::string& iniPath, SDL_Renderer* renderer)
//    : rect(rect), force(force), buttonType(buttonType), onClick(onClick) {
//    iconPath = readIni(iniPath, buttonType);
//    texture = IMG_LoadTexture(renderer, iconPath.c_str());
//    if (texture == nullptr) {
//        std::cerr << "Error loading texture: " << SDL_GetError() << "\n";
//    }
//}
//
//Button::~Button() {
//    if (texture) {
//        SDL_DestroyTexture(texture);
//    }
//}
//
//std::string Button::readIni(const std::string& iniPath, SimulationObjectType buttonType) {
//    INIReader reader(iniPath);
//
//    if (reader.ParseError() < 0) {
//        std::cerr << "Error: Can't load INI file: " << iniPath << "\n";
//        return "default_value";
//    }
//
//    if (buttonType == SimulationObjectType::Aircraft)
//        return reader.Get("ICON", "aircraft", "default_value");
//    else if (buttonType == SimulationObjectType::Waypoint)
//        return reader.Get("ICON", "waypoint", "default_value");
//    else if (buttonType == SimulationObjectType::Unknown)
//        return reader.Get("ICON", "plan", "default_value");
//
//    return "default_value";
//}
//
//void Button::render(SDL_Renderer* renderer) const {
//    if (!texture) return;
//
//    SDL_Rect renderQuad = { rect.x, rect.y, rect.w, rect.h };
//    SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
//
//    // Draw button border
//    SDL_RenderDrawRect(renderer, &rect);
//}
//
//bool Button::isClicked(int x, int y) const {
//    return x >= rect.x && x <= rect.x + rect.w &&
//        y >= rect.y && y <= rect.y + rect.h;
//}
