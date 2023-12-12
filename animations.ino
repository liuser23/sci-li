#include "animations.h"

SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0;
uint8_t gHue = 0;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void randomPatternLoop(CRGB leds[5][NUM_LEDS]) {
  for (int i = 0; i < 5; i++) {
    gPatterns[gCurrentPatternNumber](leds[i]);
  }
  
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }
  EVERY_N_SECONDS(10) {
    nextPattern();
  }
}

void rainbowLoop(CRGB leds[5][NUM_LEDS]) {
  loopSpecificPattern(leds, 0);
}

void confettiLoop(CRGB leds[5][NUM_LEDS]) {
  loopSpecificPattern(leds, 2);
}

void loopSpecificPattern(CRGB leds[5][NUM_LEDS], int index) {
  for (int i = 0; i < 5; i++) {
    gPatterns[index](leds[i]);
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }
}

void rainbow(CRGB leds[]) {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter(CRGB leds[]) {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds);
  addGlitter(leds, 80);
}

void addGlitter(CRGB leds[], fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void confetti(CRGB leds[]) {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon(CRGB leds[]) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm(CRGB leds[]) {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) {  //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle(CRGB leds[]) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}