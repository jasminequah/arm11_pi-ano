CFLAGS = -Wall -pedantic -Werror
LFLAGS = `sdl2-config --libs` -lSDL2 -lSDL2_image -lSDL2_mixer
CC     = gcc

piano_pi:
	$(CC) $(CFLAGS) piano_pi.c -o piano_pi $(LFLAGS) -lwiringPi

piano_keyboard:
	$(CC) $(CFLAGS) piano_keyboard.c -o piano_keyboard $(LFLAGS)
