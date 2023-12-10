#include "animations.h"
#include "game.h"

FASTLED_USING_NAMESPACE

//#define CLK_PIN   4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS 50
#define FRAMES_PER_SECOND 80

typedef enum {
  sOFF = 0,
  sDisplayPatterns = 1,
  sDisplayGame = 2,
} displayState;


CRGB leds[5][NUM_LEDS];
void setup() {
  delay(3000);  // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, PIN_COL0, COLOR_ORDER>(leds[0], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_COL1, COLOR_ORDER>(leds[1], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_COL2, COLOR_ORDER>(leds[2], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_COL3, COLOR_ORDER>(leds[3], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_COL4, COLOR_ORDER>(leds[4], NUM_LEDS).setCorrection(TypicalLEDStrip);

  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // FastLED.clear();  // initialize in OFF state
  // FastLED.show();
}

void loop() {
  static displayState currState = sDisplayGame;
  static bool firstRun = true;
  int exampleSnakeCoords[6][2] = {
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 1, 3 },
    { 2, 3 }
  };
  switch (currState) {
    case sOFF:
      FastLED.clear();  // turn all LEDs OFF
      FastLED.show();
      Serial.println("Off State");
      break;
    case sDisplayPatterns:
      randomPatternLoop(leds);
      break;
    case sDisplayGame:
      // Serial.println("Displaying Game");
      if (firstRun) {
        initializeGame();
        firstRun = false;
      }
      displayGame();
      break;
    default:
      Serial.println("Invalid/Unexpected State");
  }
}



