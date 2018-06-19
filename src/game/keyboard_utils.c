#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "definitions.h"
#include "piano_utils.h"
#include "keyboard_utils.h"

void init_piano_audio(piano_key_t *key) {
  key->audio = NULL;

  switch(key->note) {
    case C:
      key->audio = Mix_LoadWAV("sound/c_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case C_SHARP:
      key->audio = Mix_LoadWAV("sound/c_sharp_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case D:
      key->audio = Mix_LoadWAV("sound/d_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case D_SHARP:
      key->audio = Mix_LoadWAV("sound/d_sharp_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case E:
      key->audio = Mix_LoadWAV("sound/e_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case F:
      key->audio = Mix_LoadWAV("sound/f_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case F_SHARP:
      key->audio = Mix_LoadWAV("sound/f_sharp_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case G:
      key->audio = Mix_LoadWAV("sound/g_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case G_SHARP:
      key->audio = Mix_LoadWAV("sound/g_sharp_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

    case A:
      key->audio = Mix_LoadWAV("sound/a_note.wav");
      if (key->audio == NULL) {
        printf("Unable to load WAV file: %s\n", Mix_GetError());
      }
      break;

  }

}

void init_keyboard(keyboard_t *keyboard) {

  for (int i = 0; i < NUM_KEYS; i++) {
    piano_key_t key;
    key.note = (note_t) i;
    init_piano_audio(&key);
    keyboard->keys[i] = key;
  }
}

void play_audio(piano_key_t *key) {
  int channel;

  channel = Mix_PlayChannel(-1, key->audio, 0);
  if (channel == -1) {
    printf("Unable to play WAV file: %s\n", Mix_GetError());
  }
}

void check_keys(keyboard_t *keyboard, const char *key, SDL_Renderer *renderer) {
   if(!strcmp(key, "A")) {
      play_audio(&keyboard->keys[C]);
      display_note(renderer, C);
    }

    if(!strcmp(key, "S")) {
      play_audio(&keyboard->keys[D]);
      display_note(renderer, D);
    }

    if(!strcmp(key, "D")) {
      play_audio(&keyboard->keys[E]);
      display_note(renderer, E);
    }

    if(!strcmp(key, "F")) {
      play_audio(&keyboard->keys[F]);
      display_note(renderer, F);
    }

    if(!strcmp(key, "G")) {
      play_audio(&keyboard->keys[G]);
      display_note(renderer, G);
    }

    if(!strcmp(key, "H")) {
      play_audio(&keyboard->keys[A]);
      display_note(renderer, A);
    }

    if(!strcmp(key, "W")) {
      play_audio(&keyboard->keys[C_SHARP]);
      display_note(renderer, C_SHARP);
    }

    if (!strcmp(key, "E")) {
      play_audio(&keyboard->keys[D_SHARP]);
      display_note(renderer, D_SHARP);
    }

    if (!strcmp(key, "T")) {
      play_audio(&keyboard->keys[F_SHARP]);
      display_note(renderer, F_SHARP);
    }

    if (!strcmp(key, "Y")) {
      play_audio(&keyboard->keys[G_SHARP]);
      display_note(renderer, G_SHARP);
    }
}
