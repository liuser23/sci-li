// animations.h
#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>

#define NUM_LEDS 11

typedef void (*SimplePatternList[])(CRGB[]);

void rainbow(CRGB leds[]);
void rainbowWithGlitter(CRGB leds[]);
void addGlitter(CRGB leds[], fract8 chanceOfGlitter);
void confetti(CRGB leds[]);
void sinelon(CRGB leds[]);
void bpm(CRGB leds[]);
void juggle(CRGB leds[]);

void randomPatternLoop(CRGB leds[][NUM_LEDS]);

#endif // ANIMATIONS_H