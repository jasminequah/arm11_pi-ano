#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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

#endif
