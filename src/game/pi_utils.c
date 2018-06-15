#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <wiringPi.h>
#include "piano_utils.h"
#include "pi_utils.h"

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
