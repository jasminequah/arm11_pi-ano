#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "definitions.h"
#include "pi_utils.h"
#include "piano_utils.h"

void free_all(keyboard_t *keyboard) {

  for (int i = 0; i < NUM_KEYS; i++) {
    Mix_FreeChunk(keyboard->keys[i].audio);
  }
  free(keyboard);

}

int main (int argc, char **argv) {
  SDL_Window *window          = NULL; // main window
  SDL_Texture *background_img = NULL;
  SDL_Texture *piano_img      = NULL;
  SDL_Renderer* renderer      = NULL;
  SDL_Rect texr;
  texr.w = SCREEN_WIDTH;
  texr.h = SCREEN_HEIGHT;
  SDL_Event event;
  keyboard_t *keyboard = malloc(sizeof(keyboard_t));
  int running = 1;

  if (!init_gui(&window, &renderer)) {
    return EXIT_FAILURE;
  }

  background_img = IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH);
  if (background_img == NULL) {
    printf("Failed to load background image.\n");
  }
  SDL_RenderCopy(renderer, background_img, NULL, &texr);

  if (!load_menu(&renderer, &running)) {
    return EXIT_FAILURE;
  }

  init_keyboard(keyboard);

  piano_img = IMG_LoadTexture(renderer, PIANO_IMG_PATH);
  if (piano_img == NULL) {
    printf("Failed to load piano image.\n");
  }

  if (wiringPiSetup() == -1) {
    printf("Error: setup wiringPi failed\n");
    return EXIT_FAILURE;
  }

  while (running) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"Q") == 0 || strcmp(key, "q") == 0) {
          running = 0;
        } else if (strcmp(key, "L") == 0 || strcmp(key, "l") == 0) {
	        run_lesson(keyboard);
	        break;
      	}
      }
    }

    check_pins(keyboard, renderer);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background_img, NULL, &texr);
    SDL_RenderCopy(renderer, piano_img, NULL, &texr);
    SDL_RenderPresent(renderer);

  }

  while(Mix_Playing(0) != 0);
  free_all(keyboard);
  Mix_CloseAudio();
  SDL_DestroyTexture(background_img);
  SDL_DestroyTexture(piano_img);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;

}
