#ifndef PI_UTILS_H
#define PI_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <wiringPi.h>
#include "piano_utils.h"

typedef struct key {
  note_t    note;
  Mix_Chunk *audio;
  wiring_keys led;
  wiring_keys button;
} piano_key_t;

typedef struct keyboard {
  piano_key_t keys[NUM_KEYS];
} keyboard_t;

void init_pins(void);
void check_pins(keyboard_t *keyboard, SDL_Renderer *renderer);
void set_pi_io(keyboard_t *keyboard);
void init_piano_audio(piano_key_t *key);
void play_audio(piano_key_t *key);
void init_keyboard(keyboard_t *keyboard);
void lesson_note(keyboard_t *keyboard, note_t note);
void run_lesson(keyboard_t *keyboard);

#endif
