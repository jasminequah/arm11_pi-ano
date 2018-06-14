#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define NUM_KEYS 10
#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900
#define PIANO_IMG_PATH "graphics/piano.PNG"
#define MENU_IMG_PATH "graphics/menu.PNG"
#define C_MARKER_PATH "graphics/c_marker.PNG"
#define D_MARKER_PATH "graphics/d_marker.PNG"
#define E_MARKER_PATH "graphics/e_marker.PNG"
#define F_MARKER_PATH "graphics/f_marker.PNG"
#define G_MARKER_PATH "graphics/g_marker.PNG"
#define A_MARKER_PATH "graphics/a_marker.PNG"
#define C_SHARP_MARKER_PATH "graphics/c_sharp_marker.PNG"
#define D_SHARP_MARKER_PATH "graphics/d_sharp_marker.PNG"
#define F_SHARP_MARKER_PATH "graphics/f_sharp_marker.PNG"
#define G_SHARP_MARKER_PATH "graphics/g_sharp_marker.PNG"

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

int randCol() {
  return rand() % 255 + 0;
}

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
  
void display_note(SDL_Renderer *renderer, note_t note) {
  SDL_Texture *note_marker;  
  SDL_Rect texr;
  switch(note) {
    case C:
      note_marker = IMG_LoadTexture(renderer, C_MARKER_PATH);
      break;
    case D:
      note_marker = IMG_LoadTexture(renderer, D_MARKER_PATH);
      break;
    case E:
      note_marker = IMG_LoadTexture(renderer, E_MARKER_PATH);
      break;
    case F:
      note_marker = IMG_LoadTexture(renderer, F_MARKER_PATH);
      break;
    case G:
      note_marker = IMG_LoadTexture(renderer, G_MARKER_PATH);
      break;
    case A:
      note_marker = IMG_LoadTexture(renderer, A_MARKER_PATH);
      break;
    case C_SHARP:
      note_marker = IMG_LoadTexture(renderer, C_SHARP_MARKER_PATH);
      break;
    case D_SHARP:
      note_marker = IMG_LoadTexture(renderer, D_SHARP_MARKER_PATH);
      break;
    case F_SHARP:
      note_marker = IMG_LoadTexture(renderer, F_SHARP_MARKER_PATH);
      break;
    case G_SHARP:
      note_marker = IMG_LoadTexture(renderer, G_SHARP_MARKER_PATH);
      break;
  }
  if (note_marker == NULL) {
    printf("Failed to load piano image.\n");
  }
// May not need
  int w, h;
  SDL_QueryTexture(note_marker, NULL, NULL, &w, &h);

  texr.x = SCREEN_WIDTH / 2 - (w * 0.6 )/ 2;
  texr.y = SCREEN_HEIGHT / 2 - (h)/ 2 - 100;
  texr.w = w * 0.6;
  texr.h = h;

  SDL_RenderCopy(renderer, note_marker, NULL, &texr);
  SDL_RenderPresent(renderer);
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
  }
  free(keyboard);

}

int load_menu(SDL_Renderer *renderer, SDL_Rect texr, int *running, int *w, int *h) {

  SDL_Texture *menu_img = NULL;
  SDL_Event event;

  if (renderer == NULL) {
    printf("renderer is null\n");
  }

  menu_img = IMG_LoadTexture(renderer, MENU_IMG_PATH);
  if (menu_img == NULL) {
    printf("Failed to load menu image.\n");
    return 0;
  }

  SDL_QueryTexture(menu_img, NULL, NULL, w, h);
  texr.x = SCREEN_WIDTH / 2;
  texr.y = SCREEN_HEIGHT / 2;
  texr.w = *w;
  texr.h = *h;

  SDL_RenderCopy(renderer, menu_img, NULL, &texr);
  SDL_RenderPresent(renderer);

  SDL_WaitEvent(&event);
  while (event.type != SDL_KEYDOWN) {
    SDL_WaitEvent(&event);
  }
  const char *key = SDL_GetKeyName(event.key.keysym.sym);
  if (strcmp(key, "Q") == 0 || strcmp(key, "q") == 0) {
    //quit
    *running = 0;
  }
  SDL_DestroyTexture(menu_img);
  return 1;

}
/*
int init_gui(SDL_Window **window, SDL_Renderer **renderer) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("Error: failed to initialise SDL.\n");
    return 0;
  }

  if (init_audio()) {
    printf("Error: failed to initialise audio: %s\n", Mix_GetError());
    return 0;
  }

  *window   = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  return 1;

}
*/
int main (int argc, char **argv) {
  SDL_Window *window     = NULL; // main window
  SDL_Texture *piano_img = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Rect texr;
  SDL_Event event;
  int w, h;
  keyboard_t *keyboard = malloc(sizeof(keyboard_t));
  int running = 1;

  //srand(time(NULL)); 

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("Error: failed to initialise SDL.\n");
    return EXIT_FAILURE;
  }

  if (init_audio()) {
    printf("Error: failed to initialise audio: %s\n", Mix_GetError());
    return EXIT_FAILURE;
  }

  window   = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

/*
  if (!init_gui(&window, &renderer)) {
    return EXIT_FAILURE;
  } 
*/
  if (!load_menu(renderer, texr, &running, &w, &h)) {
    return EXIT_FAILURE;
  }

  init_keyboard(keyboard);

  piano_img = IMG_LoadTexture(renderer, PIANO_IMG_PATH);
  if (piano_img == NULL) {
    printf("Failed to load piano image.\n");
  }

  SDL_QueryTexture(piano_img, NULL, NULL, &w, &h);

  texr.x = SCREEN_WIDTH / 2 - (w * 0.6 )/ 2;
  texr.y = SCREEN_HEIGHT / 2 - (h * 0.6)/ 2;
  texr.w = w * 0.6;
  texr.h = h * 0.6;

  SDL_SetRenderDrawColor(renderer, randCol(), randCol(), randCol(), 255);

  SDL_RenderCopy(renderer, piano_img, NULL, &texr);
  SDL_RenderPresent(renderer);

  while (running) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"Q") == 0 || strcmp(key, "q") == 0) {
          running = 0;
        } else {
          check_keys(keyboard, key, renderer);
        }
      }
    }

    /*new_time = SDL_GetTicks();
    if (new_time - old_time > change_color_time) {
      SDL_SetRenderDrawColor(renderer, randCol(), randCol(), randCol(), 255);
      old_time = new_time;
    } */

    SDL_RenderCopy(renderer, piano_img, NULL, &texr);
    SDL_RenderPresent(renderer);
  }

  while(Mix_Playing(0) != 0);
  free_all(keyboard); //need to free sound, keys, keyboard, etc.  
  Mix_CloseAudio();
  SDL_DestroyTexture(piano_img);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}


