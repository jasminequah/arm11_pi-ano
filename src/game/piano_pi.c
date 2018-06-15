#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wiringPi.h>

#define NUM_KEYS 10
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 450
#define BACKGROUND_IMG_PATH "graphics/background.png"
#define PIANO_IMG_PATH "graphics/piano.png"
#define MENU_IMG_PATH "graphics/menu.png"
#define C_MARKER_PATH "graphics/c_marker.png"
#define D_MARKER_PATH "graphics/d_marker.png"
#define E_MARKER_PATH "graphics/e_marker.png"
#define F_MARKER_PATH "graphics/f_marker.png"
#define G_MARKER_PATH "graphics/g_marker.png"
#define A_MARKER_PATH "graphics/a_marker.png"
#define C_SHARP_MARKER_PATH "graphics/c_sharp_marker.png"
#define D_SHARP_MARKER_PATH "graphics/d_sharp_marker.png"
#define F_SHARP_MARKER_PATH "graphics/f_sharp_marker.png"
#define G_SHARP_MARKER_PATH "graphics/g_sharp_marker.png"

typedef enum {
  C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A
} note_t;

typedef enum {
  LedPin = 0,
  ButtonPin = 7,
  LedPinTwo = 6,
  ButtonPinTwo = 1,
  LedPinThree = 2,
  ButtonPinThree = 5,
  LedPinFour = 4,
  ButtonPinFour = 3,
  LedPinFive = 11,
  ButtonPinFive = 10,
  LedPinSix = 8,
  ButtonPinSix = 16,
  blackKey1 = 14,
  blackKey2 = 15,
  blackKey3 = 13,
  blackKey4 = 12
} wiring_keys;

typedef struct key {
  note_t    note;
  Mix_Chunk *audio;
  wiring_keys led;
  wiring_keys button;
} piano_key_t;

typedef struct keyboard {
  piano_key_t keys[NUM_KEYS];
} keyboard_t;

void play_audio(piano_key_t *key);

int randCol() {
  return rand() % 255 + 0;
}

void init_pins(void) {
  pinMode(LedPin, OUTPUT);
  pinMode(LedPinTwo, OUTPUT);
  pinMode(LedPinThree, OUTPUT);
  pinMode(LedPinFour, OUTPUT);
  pinMode(LedPinFive, OUTPUT);
  pinMode(LedPinSix, OUTPUT);

  pinMode(ButtonPin, INPUT);
  pinMode(ButtonPinTwo, INPUT);
  pinMode(ButtonPinThree, INPUT);
  pinMode(ButtonPinFour, INPUT);
  pinMode(ButtonPinFive, INPUT);
  pinMode(ButtonPinSix, INPUT);
  pinMode(blackKey1, INPUT);
  pinMode(blackKey2, INPUT);
  pinMode(blackKey3, INPUT);
  pinMode(blackKey4, INPUT);

  pullUpDnControl(ButtonPin, PUD_UP);
  pullUpDnControl(ButtonPinTwo, PUD_UP);
  pullUpDnControl(ButtonPinThree, PUD_UP);
  pullUpDnControl(ButtonPinFour, PUD_UP);
  pullUpDnControl(ButtonPinFive, PUD_UP);
  pullUpDnControl(ButtonPinSix, PUD_UP);
  pullUpDnControl(blackKey1, PUD_UP);
  pullUpDnControl(blackKey2, PUD_UP);
  pullUpDnControl(blackKey3, PUD_UP);
  pullUpDnControl(blackKey4, PUD_UP);
}

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

void check_pins(keyboard_t *keyboard, SDL_Renderer *renderer) {
    if(digitalRead(keyboard->keys[C].button) == 0) {
      digitalWrite(keyboard->keys[C].led, LOW);
      display_note(renderer, C);
      play_audio(&keyboard->keys[C]);
    } else {
      digitalWrite(keyboard->keys[C].led, HIGH);
    }

    if(digitalRead(keyboard->keys[D].button) == 0) {
      digitalWrite(keyboard->keys[D].led, LOW);
      display_note(renderer, D);
      play_audio(&keyboard->keys[D]);
    } else {
      digitalWrite(keyboard->keys[D].led, HIGH);
    }

    if(digitalRead(keyboard->keys[E].button) == 0) {
      digitalWrite(keyboard->keys[E].led, LOW);
      display_note(renderer, E);
      play_audio(&keyboard->keys[E]);
    } else {
      digitalWrite(keyboard->keys[E].led, HIGH);
    }

    if(digitalRead(keyboard->keys[F].button) == 0) {
      digitalWrite(keyboard->keys[F].led, LOW);
      display_note(renderer, F);
      play_audio(&keyboard->keys[F]);
    } else {
      digitalWrite(keyboard->keys[F].led, HIGH);
    }

    if(digitalRead(keyboard->keys[G].button) == 0) {
      digitalWrite(keyboard->keys[G].led, LOW);
      display_note(renderer, G);
      play_audio(&keyboard->keys[G]);
    } else {
      digitalWrite(keyboard->keys[G].led, HIGH);
    }

    if(digitalRead(keyboard->keys[A].button) == 0) {
      digitalWrite(keyboard->keys[A].led, LOW);
      display_note(renderer, A);
      play_audio(&keyboard->keys[A]);
    } else {
      digitalWrite(keyboard->keys[A].led, HIGH);
    }

    if(digitalRead(keyboard->keys[C_SHARP].button) == 0) {
      display_note(renderer, C_SHARP);
      play_audio(&keyboard->keys[C_SHARP]);
    }

    if(digitalRead(keyboard->keys[D_SHARP].button) == 0) {
      display_note(renderer, D_SHARP);
      play_audio(&keyboard->keys[D_SHARP]);
    }

    if(digitalRead(keyboard->keys[F_SHARP].button) == 0) {
      display_note(renderer, F_SHARP);
      play_audio(&keyboard->keys[F_SHARP]);
    }

    if(digitalRead(keyboard->keys[G_SHARP].button) == 0) {
      display_note(renderer, G_SHARP);
      play_audio(&keyboard->keys[G_SHARP]);
    }
}


void init_piano_audio(piano_key_t *key) {

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


void set_pi_io(keyboard_t *keyboard) {

  keyboard->keys[C].button = ButtonPin;
  keyboard->keys[C].led    = LedPin;
  keyboard->keys[D].button = ButtonPinTwo;
  keyboard->keys[D].led    = LedPinTwo;
  keyboard->keys[E].button = ButtonPinThree;
  keyboard->keys[E].led    = LedPinThree;
  keyboard->keys[F].button = ButtonPinFour;
  keyboard->keys[F].led    = LedPinFour;
  keyboard->keys[G].button = ButtonPinFive;
  keyboard->keys[G].led    = LedPinFive;
  keyboard->keys[A].button = ButtonPinSix;
  keyboard->keys[A].led    = LedPinSix;
  keyboard->keys[C_SHARP].button = blackKey1;
  keyboard->keys[D_SHARP].button = blackKey2;
  keyboard->keys[F_SHARP].button = blackKey3;
  keyboard->keys[G_SHARP].button = blackKey4;

}

void init_keyboard(keyboard_t *keyboard) {

  for (int i = 0; i < NUM_KEYS; i++) {
    piano_key_t key;
    key.note = (note_t) i;
    init_piano_audio(&key);
    keyboard->keys[i] = key;
  }
  set_pi_io(keyboard);
}


void play_audio(piano_key_t *key) {

  int channel;

  if (Mix_Playing(1) == 0) {
  channel = Mix_PlayChannel(-1, key->audio, 0);
  if (channel == -1) {
    printf("Unable to play WAV file: %s\n", Mix_GetError());
  }
  }

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

void free_all(keyboard_t *keyboard) {

  for (int i = 0; i < NUM_KEYS; i++) {
    Mix_FreeChunk(keyboard->keys[i].audio);
  }
  free(keyboard);

}

void lesson_note(keyboard_t *keyboard, note_t note) {
  while(digitalRead(keyboard->keys[note].button)) {
    digitalWrite(keyboard->keys[note].led, LOW);
  }
  play_audio(&keyboard->keys[note]);
  digitalWrite(keyboard->keys[note].led, HIGH);
  while(Mix_Playing(0) != 0);
}

void run_lesson(keyboard_t *keyboard) {

  lesson_note(keyboard, E);
  lesson_note(keyboard, D);
  lesson_note(keyboard, C);
  lesson_note(keyboard, D);
  lesson_note(keyboard, E);
  lesson_note(keyboard, E);
  lesson_note(keyboard, E);
  lesson_note(keyboard, D);
  lesson_note(keyboard, D);
  lesson_note(keyboard, D);
  lesson_note(keyboard, E);
  lesson_note(keyboard, G);
  lesson_note(keyboard, G);

  lesson_note(keyboard, E);
  lesson_note(keyboard, D);
  lesson_note(keyboard, C);
  lesson_note(keyboard, D);
  lesson_note(keyboard, E);
  lesson_note(keyboard, E);
  lesson_note(keyboard, E);
  lesson_note(keyboard, D);
  lesson_note(keyboard, D);
  lesson_note(keyboard, E);
  lesson_note(keyboard, D);
  lesson_note(keyboard, C);

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

int main (int argc, char **argv) {
  SDL_Window *window     = NULL; // main window
  SDL_Texture *background_img = NULL;
  SDL_Texture *piano_img = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Rect texr;
  texr.w = SCREEN_WIDTH;
  texr.h = SCREEN_HEIGHT;
  SDL_Event event;
  keyboard_t *keyboard = malloc(sizeof(keyboard_t));
  int running = 1;

  if (!init_gui(&window, &renderer)) {
    return EXIT_FAILURE;
  }

  init_keyboard(keyboard);

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
