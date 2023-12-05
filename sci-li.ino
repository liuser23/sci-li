#include "animations.h"
#include "game.h"

FASTLED_USING_NAMESPACE

//#define CLK_PIN   4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS 80
#define FRAMES_PER_SECOND 80

typedef enum {
  sOFF = 0,
  sDisplayPatterns = 1,
  sDisplayGame = 2,
} displayState;

// idea taken from lab6, prob other useful structs there
// prob will later move these into game.ino/game.h
typedef struct {
  uint8_t x;
  uint8_t y;
} xy;


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
      Serial.println("Displaying Game");
      exampleColorSnake(exampleSnakeCoords, 6);
      FastLED.show();
      break;
    default:
      Serial.println("Invalid/Unexpected State");
  }
}


xy calculateXY(int row, int col) {
  // max_height - row
  uint8_t x = col;
  uint8_t y = 14 - row;
  return { x, y };
}

void testColorPoint(int row, int col, CRGB color) {
  xy ledLocation = calculateXY(row, col);
  leds[ledLocation.x][ledLocation.y] = color;
}

void exampleColorSnake(int snakeCoords[][2], size_t snakeSize) {
  for (size_t i = 0; i < snakeSize; i++) {
    testColorPoint(snakeCoords[i][0], snakeCoords[i][1], CRGB::White);
  }
}
