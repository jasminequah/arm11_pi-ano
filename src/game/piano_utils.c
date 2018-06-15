#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "definitions.h"

void display_note(SDL_Renderer *renderer, note_t note) {
  SDL_Texture *note_marker;
  SDL_Rect texr;
  texr.w = SCREEN_WIDTH;
  texr.h = SCREEN_HEIGHT;
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

  SDL_RenderCopy(renderer, note_marker, NULL, &texr);
  SDL_RenderPresent(renderer);
}

int init_audio(void) {

  int audio_rate = 44100;
  Uint16 audio_format = AUDIO_S16SYS; // Bitrate of WAV faile
  int audio_channels = 1;  // 2 for stereo, 1 for mono
  int audio_buffers = 1024;

  if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
    return 1;
  }

  return 0;

}

int load_menu(SDL_Renderer **renderer, int *running) {

  SDL_Texture *menu_img = NULL;
  SDL_Event event;
  SDL_Rect texr;
  texr.w = SCREEN_WIDTH;
  texr.h = SCREEN_HEIGHT;

  menu_img = IMG_LoadTexture(*renderer, MENU_IMG_PATH);
  if (menu_img == NULL) {
    printf("Failed to load menu image.\n");
    return 0;
  }

  SDL_RenderCopy(*renderer, menu_img, NULL, &texr);
  SDL_RenderPresent(*renderer);

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
