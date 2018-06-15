#ifndef PIANO_UTILS_H
#define PIANO_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "definitions.h"

void display_note(SDL_Renderer *renderer, note_t note);
int init_audio(void);
int load_menu(SDL_Renderer **renderer, int *running);
int init_gui(SDL_Window **window, SDL_Renderer **renderer);

#endif
