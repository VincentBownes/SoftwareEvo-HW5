#define SOL_ALL_SAFTIES_ON 1
#include "Game.hpp"

#include <iostream>
#include "SDL2/SDL_image.h"

#include "sdl_utils.hpp"
#include "gameobjects/Chopper.hpp"
#include "gameobjects/Tank.hpp"
#include "gameobjects/Pacman.hpp"

SDL_Renderer* Game::renderer{};
SDL_Window* Game::window{};
sol::state Game::lua{};

Game::Game(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{

   //initialize sol::state, open libraries and deal with lua config script
   lua.open_libraries(sol::lib::base, sol::lib::package);
   lua.script_file("config.lua");

   Uint32 flags{};
   if (fullscreen) {
      flags = SDL_WINDOW_FULLSCREEN;
   }

   if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
      std::cout << "Subsystems initialized..." << std::endl;
      window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
      if (window) {
         std::cout << "Window created..." << std::endl;
      }
      renderer = SDL_CreateRenderer(window, -1, 0);
      if (renderer) {
         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
         std::cout << "Renderer created..." << std::endl;
      }
   is_running = true;
   } else {
      is_running = false;
   }
}

Game::~Game()
{
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}

void Game::load_level()
{
   sol::lua_table table = lua["gameobjs"];
   // I wanted to use size in the for loop but its always 0??
   const std::size_t size = table.size();

   for(const auto& pair : table){
      sol::object key = pair.first;
      sol::object value = pair.second;

      sol::type t = value.get_type();
      std::string kind = static_cast<std::string>(table[key][value]);
      float xpos = static_cast<float>(table[key][value]);
      float ypos = static_cast<float>(table[key][value]);
      float xvel = static_cast<float>(table[key][value]);
      float yvel = static_cast<float>(table[key][value]);

      if(kind == "chopper"){
         auto chopper = std::make_unique<Chopper>(xpos, ypos, xvel, yvel);
         game_objs.emplace_back(std::move(chopper));
      }
      else if(kind == "pacman"){
         auto pacman = std::make_unique<Pacman>(xpos, ypos, xvel, yvel);
         game_objs.emplace_back(std::move(pacman));
      }
      else if(kind == "tank"){
         auto tank = std::make_unique<Tank>(xpos, ypos, xvel, yvel);
         game_objs.emplace_back(std::move(tank));
      }
   }
}

void Game::handle_events()
{
   SDL_Event event;
   SDL_PollEvent(&event);
   switch (event.type) {
      case SDL_QUIT:
         is_running = false;
         break;
      default:
         break;
   }
}

void Game::update(const float dt)
{
   for (auto& i: game_objs) {
      i->update(dt);
   }
}

void Game::render()
{
   SDL_RenderClear(renderer);
   for (auto& i: game_objs) {
      i->render();
   }
   SDL_RenderPresent(renderer);
}

