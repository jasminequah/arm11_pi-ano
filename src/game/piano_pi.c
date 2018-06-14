#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wiringPi.h>

#define NUM_KEYS 10

typedef enum {
  C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A
} note_t;

typedef struct key {
  note_t    note;
  int       is_pressed;
  Mix_Chunk *audio;
} piano_key_t;

typedef struct keyboard {
  piano_key_t keys[NUM_KEYS];
} keyboard_t;

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

/* White Keys */
#define LedPin 0 //red
#define ButtonPin 7 

#define LedPinTwo 6 //green
#define ButtonPinTwo 1

#define LedPinThree 2 //yellow
#define ButtonPinThree 5

#define LedPinFour 4 // blue
#define ButtonPinFour 3

#define LedPinFive 11 //red
#define ButtonPinFive 10

#define LedPinSix 8 //oranges
#define ButtonPinSix 16

/* Black keys */
#define blackKey1 14
#define blackKey2 15
#define blackKey3 13
#define blackKey4 12


void play_audio(piano_key_t *key);

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900
#define IMG_PATH "graphics/temppiano.PNG"

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

void check_pins(keyboard_t *keyboard) {
    if(digitalRead(ButtonPin) == 0) {
      digitalWrite(LedPin, LOW);
      keyboard->keys[C].is_pressed = 1; 
      play_audio(&keyboard->keys[C]);
    } else {
      digitalWrite(LedPin, HIGH);
      keyboard->keys[C].is_pressed = 0;
    }

    if(digitalRead(ButtonPinTwo) == 0) {
      digitalWrite(LedPinTwo, LOW);
      keyboard->keys[D].is_pressed = 1; 
      play_audio(&keyboard->keys[D]);
    } else {
      digitalWrite(LedPinTwo, HIGH);
      keyboard->keys[D].is_pressed = 0;
    }

    if(digitalRead(ButtonPinThree) == 0) {
      digitalWrite(LedPinThree, LOW);
      keyboard->keys[E].is_pressed = 1; 
      play_audio(&keyboard->keys[E]);
    } else {
      digitalWrite(LedPinThree, HIGH);
      keyboard->keys[E].is_pressed = 0;
    }

    if(digitalRead(ButtonPinFour) == 0) {
      digitalWrite(LedPinFour, LOW);
      keyboard->keys[F].is_pressed = 1; 
      play_audio(&keyboard->keys[F]);
    } else {
      digitalWrite(LedPinFour, HIGH);
      keyboard->keys[F].is_pressed = 0;
    }

    if(digitalRead(ButtonPinFive) == 0) {
      digitalWrite(LedPinFive, LOW);
      keyboard->keys[G].is_pressed = 1; 
      play_audio(&keyboard->keys[G]);
    } else {
      digitalWrite(LedPinFive, HIGH);
      keyboard->keys[G].is_pressed = 0;
    }

    if(digitalRead(ButtonPinSix) == 0) {
      digitalWrite(LedPinSix, LOW);
      keyboard->keys[A].is_pressed = 1; 
      play_audio(&keyboard->keys[A]);
    } else {
      digitalWrite(LedPinSix, HIGH);
      keyboard->keys[A].is_pressed = 0;
    }

    if(digitalRead(blackKey1) == 0) {
      keyboard->keys[C_SHARP].is_pressed = 1; 
      play_audio(&keyboard->keys[C_SHARP]);
    } else {
      keyboard->keys[C_SHARP].is_pressed = 0;
    }

    if (digitalRead(blackKey2) == 0) {
      keyboard->keys[D_SHARP].is_pressed = 1; 
      play_audio(&keyboard->keys[D_SHARP]);
    } else {
      keyboard->keys[D_SHARP].is_pressed = 0;
    }

    if (digitalRead(blackKey3) == 0) {
      keyboard->keys[F_SHARP].is_pressed = 1;
      play_audio(&keyboard->keys[F_SHARP]);
    } else {
      keyboard->keys[F_SHARP].is_pressed = 0;
    }

    if (digitalRead(blackKey4) == 0) {
      keyboard->keys[G_SHARP].is_pressed = 1; 
      play_audio(&keyboard->keys[G_SHARP]);
    } else {
      keyboard->keys[G_SHARP].is_pressed = 0;
    }
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
 
  if (Mix_Playing(1) == 0) { 
  channel = Mix_PlayChannel(-1, key->audio, 0);
  if (channel == -1) {
    printf("Unable to play WAV file: %s\n", Mix_GetError());
  }
  }

}

int init_audio(void) {

  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS; // Bitrate of WAV faile
  int audio_channels = 1;  // 2 for stereo, 1 for mono
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
  
  window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

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


  if (wiringPiSetup() == -1) {
    printf("Setup wiringPi failed\n");
    return 1;
  }

  while (running) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"Q") == 0 || strcmp(key, "q") == 0) {
          running = 0;
        }
      }
    }
   
    check_pins(keyboard);
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, pianoImg, NULL, &texr);
    SDL_RenderPresent(renderer);  

    /*new_time = SDL_GetTicks();
    if (new_time - old_time > change_color_time) {
      SDL_SetRenderDrawColor(renderer, randCol(), randCol(), randCol(), 255);
      old_time = new_time;
    } */

  }

  free_all(keyboard); 
  Mix_CloseAudio();
  SDL_DestroyTexture(pianoImg);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

