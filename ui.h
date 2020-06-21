
#include <SDL2/SDL_events.h>
static void ui_run(SDL_Window* window) {
    for (;;) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT) return;
    }
}
