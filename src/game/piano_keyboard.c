#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define NUM_KEYS 10

typedef enum {
  C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A
} note_t;

typedef struct key {
  note_t note;
  int    is_pressed;
  Mix_Chunk *audio;
} piano_key_t;

typedef struct keyboard {
  piano_key_t keys[NUM_KEYS];
} keyboard_t;

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900
#define IMG_PATH "graphics/temppiano.PNG"

int randCol() {
  return rand() % 255 + 0;
}

void init_piano_audio(piano_key_t *key) {
  key->audio = malloc(sizeof(Mix_Chunk));

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
    piano_key_t *key = malloc(sizeof(piano_key_t));
    key->note = (note_t) i;
    key->is_pressed = 0;
    init_piano_audio(key);
    keyboard->keys[i] = *key;
  }
}

void play_audio(piano_key_t *key) {
  int channel;

  channel = Mix_PlayChannel(-1, key->audio, 0);
  if (channel == -1) {
    printf("Unable to play WAV file: %s\n", Mix_GetError());
  }
}
  
void check_keys(keyboard_t *keyboard, const char *key) {
   if(!strcmp(key, "A")) {
      keyboard->keys[C].is_pressed = 1;
      play_audio(&keyboard->keys[C]);
    } else {
      keyboard->keys[C].is_pressed = 0;
    }

    if(!strcmp(key, "S")) {
      keyboard->keys[D].is_pressed = 1;
      play_audio(&keyboard->keys[D]);
    } else {
      keyboard->keys[D].is_pressed = 0;
    }

    if(!strcmp(key, "D")) {
      keyboard->keys[E].is_pressed = 1;
      play_audio(&keyboard->keys[E]);
    } else {
      keyboard->keys[E].is_pressed = 0;
    }

    if(!strcmp(key, "F")) {
      keyboard->keys[F].is_pressed = 1;
      play_audio(&keyboard->keys[F]);
    } else {
      keyboard->keys[F].is_pressed = 0;
    }

    if(!strcmp(key, "G")) {
      keyboard->keys[G].is_pressed = 1;
      play_audio(&keyboard->keys[G]);
    } else {
      keyboard->keys[G].is_pressed = 0;
    }

    if(!strcmp(key, "H")) {
      keyboard->keys[A].is_pressed = 1;
      play_audio(&keyboard->keys[A]);
    } else {
      keyboard->keys[A].is_pressed = 0;
    }

    if(!strcmp(key, "W")) {
      keyboard->keys[C_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[C_SHARP]);
    } else {
      keyboard->keys[C_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "E")) {
      keyboard->keys[D_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[D_SHARP]);
    } else {
      keyboard->keys[D_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "T")) {
      keyboard->keys[F_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[F_SHARP]);
    } else {
      keyboard->keys[F_SHARP].is_pressed = 0;
    }

    if (!strcmp(key, "Y")) {
      keyboard->keys[G_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[G_SHARP]);
    } else {
      keyboard->keys[G_SHARP].is_pressed = 0;
    }
}

int init_audio(void) {

  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS; // Bitrate of WAV file
  int audio_channels = 1; // 2 for stereo, 1 for mono
  int audio_buffers = 4096;
 
  if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
    return 1;
  }

  return 0;

}


void free_all(keyboard_t *keyboard) {

  for (int i = 0; i < NUM_KEYS; i++) {
    Mix_FreeChunk(keyboard->keys[i].audio);
    free(&keyboard->keys[i]);
  }
  free(keyboard);
}


int main (int argc, char **argv) {
  SDL_Window *window = NULL; // main window
  SDL_Texture *pianoImg = NULL;
  SDL_Renderer* renderer = NULL;
  int w, h;
  keyboard_t *keyboard = malloc(sizeof(keyboard_t));

  //srand(time(NULL)); 
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("Error: failed to initialise SDL.\n");
    return EXIT_FAILURE;
  }

  if (init_audio()) {
    printf("Error: failed to initialise audio: %s\n", Mix_GetError());
    return EXIT_FAILURE;
  }

  init_keyboard(keyboard);
  window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  pianoImg = IMG_LoadTexture(renderer, IMG_PATH);
  if (pianoImg == NULL) {
    printf("img is null\n");
  }  

  SDL_QueryTexture(pianoImg, NULL, NULL, &w, &h);

  SDL_Rect texr;
  texr.x = SCREEN_WIDTH / 2 - w / 2;
  texr.y = SCREEN_HEIGHT / 2 - h / 2;
  texr.w = w;
  texr.h = h;


  SDL_SetRenderDrawColor(renderer, randCol(), randCol(), randCol(), 255);

  int running = 1;
//  uint32_t old_time = 0, change_color_time = 1000, new_time;

  while (running) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"Q") == 0 || strcmp(key, "q") == 0) {
          running = 0;
        } else {
          check_keys(keyboard, key);
        }
      }
    }
   
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, pianoImg, NULL, &texr);
    SDL_RenderPresent(renderer);   

    /*new_time = SDL_GetTicks();
    if (new_time - old_time > change_color_time) {
      SDL_SetRenderDrawColor(renderer, randCol(), randCol(), randCol(), 255);
      old_time = new_time;
    } */

  }

  free_all(keyboard); //need to free sound, keys, keyboard, etc.  
  Mix_CloseAudio();
  SDL_DestroyTexture(pianoImg);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}


