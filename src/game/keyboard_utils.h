#ifndef KEYBOARD_UTILS_H
#define KEYBOARD_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "definitions.h"
#include "piano_utils.h"

typedef struct key {
  note_t note;
  int    is_pressed;
  Mix_Chunk *audio;
} piano_key_t;

typedef struct keyboard {
  piano_key_t keys[NUM_KEYS];
} keyboard_t;

void init_piano_audio(piano_key_t *key);
void check_keys(keyboard_t *keyboard, const char *key, SDL_Renderer *renderer);
void init_keyboard(keyboard_t *keyboard);
void play_audio(piano_key_t *key);


#endif
