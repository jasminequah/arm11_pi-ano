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
      keyboard->keys[C].is_pressed = 1;
      play_audio(&keyboard->keys[C]);
      display_note(renderer, C);
    } else {
      keyboard->keys[C].is_pressed = 0;
    }

    if(!strcmp(key, "S")) {
      keyboard->keys[D].is_pressed = 1;
      play_audio(&keyboard->keys[D]);
      display_note(renderer, D);
    } else {
      keyboard->keys[D].is_pressed = 0;
    }

    if(!strcmp(key, "D")) {
      keyboard->keys[E].is_pressed = 1;
      play_audio(&keyboard->keys[E]);
      display_note(renderer, E);
    } else {
      keyboard->keys[E].is_pressed = 0;
    }

    if(!strcmp(key, "F")) {
      keyboard->keys[F].is_pressed = 1;
      play_audio(&keyboard->keys[F]);
      display_note(renderer, F);
    } else {
      keyboard->keys[F].is_pressed = 0;
    }

    if(!strcmp(key, "G")) {
      keyboard->keys[G].is_pressed = 1;
      play_audio(&keyboard->keys[G]);
      display_note(renderer, G);
    } else {
      keyboard->keys[G].is_pressed = 0;
    }

    if(!strcmp(key, "H")) {
      keyboard->keys[A].is_pressed = 1;
      play_audio(&keyboard->keys[A]);
      display_note(renderer, A);
    } else {
      keyboard->keys[A].is_pressed = 0;
    }

    if(!strcmp(key, "W")) {
      keyboard->keys[C_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[C_SHARP]);
      display_note(renderer, C_SHARP);
    } else {
      keyboard->keys[C_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "E")) {
      keyboard->keys[D_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[D_SHARP]);
      display_note(renderer, D_SHARP);
    } else {
      keyboard->keys[D_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "T")) {
      keyboard->keys[F_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[F_SHARP]);
      display_note(renderer, F_SHARP);
    } else {
      keyboard->keys[F_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "Y")) {
      keyboard->keys[G_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[G_SHARP]);
      display_note(renderer, G_SHARP);
    } else {
      keyboard->keys[G_SHARP].is_pressed = 0;
    }
}
