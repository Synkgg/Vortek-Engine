#include "SDL_Wrappers.h"
#include <iostream>
#include <Logger/Logger.h>

void VORTEK_UTIL::SDL_Destroyer::operator()(SDL_Window* window) const
{
    SDL_DestroyWindow(window);
    VORTEK_LOG("Destroyed SDL WINDOW");
}

void VORTEK_UTIL::SDL_Destroyer::operator()(SDL_GameController* controller) const
{
    SDL_GameControllerClose(controller);
    controller = nullptr;
    VORTEK_LOG("Closed SDL Game Controller!");
}

void VORTEK_UTIL::SDL_Destroyer::operator()(Mix_Chunk* chunk) const
{
    Mix_FreeChunk(chunk);
    VORTEK_LOG("Freed SDL Mix_Chunk!");
}

void VORTEK_UTIL::SDL_Destroyer::operator()(Mix_Music* music) const
{
    Mix_FreeMusic(music);
    VORTEK_LOG("Freed SDL Mix_Chunk!");
}

void VORTEK_UTIL::SDL_Destroyer::operator()(SDL_Cursor* cursor) const
{

}