#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"

void RunWindow();

int main(int argc, char *argv[])
{
    RunWindow();
    return 0;
}

void RunWindow()
{
    #ifdef LINUX
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {

    }

    SDL_Window* window = SDL_CreateWindow("CPup", 800, 600, 0);

    if (window == NULL)
    {

    }

    bool running = true;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }
}
