#include <wiringPi.h>
#include <stdio.h>

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

int main(void) {

  if(wiringPiSetup() == -1){
    printf("setup WiringPi failed");
    return 1;
  }

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

  while(1){
    if(digitalRead(ButtonPin) == 0) {
      digitalWrite(LedPin, LOW);
    } else {
      digitalWrite(LedPin, HIGH);
    }

    if(digitalRead(ButtonPinTwo) == 0) {
      digitalWrite(LedPinTwo, LOW);
    } else {
      digitalWrite(LedPinTwo, HIGH);
    }
  
    if(digitalRead(ButtonPinThree) == 0) {
      digitalWrite(LedPinThree, LOW);
    } else {
      digitalWrite(LedPinThree, HIGH);
    }

    if(digitalRead(ButtonPinFour) == 0) {
      digitalWrite(LedPinFour, LOW);
    } else {
      digitalWrite(LedPinFour, HIGH);
    }

    if(digitalRead(ButtonPinFive) == 0) {
      digitalWrite(LedPinFive, LOW);
    } else {
      digitalWrite(LedPinFive, HIGH);
    }

    if(digitalRead(ButtonPinSix) == 0) {
      digitalWrite(LedPinSix, LOW);
    } else {
      digitalWrite(LedPinSix, HIGH);
    }
     
    if(digitalRead(blackKey1) == 0) {
      printf("C#");
    } 

    if (digitalRead(blackKey2) == 0) {
      printf("D#");
    }

    if (digitalRead(blackKey3) == 0) {
      printf("F#");
    }

    if (digitalRead(blackKey4) == 0) {
      printf("G#");
    }

  }
  return 0;
}