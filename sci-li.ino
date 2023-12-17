#include "animations.h"
#include "game.h"
#include <Arduino.h>
#include <Adafruit_SleepyDog.h>

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
  sDisplayRainbow = 3,
  sDisplayConfetti = 4,
} displayState;

displayState currState = sOFF;

CRGB leds[5][NUM_LEDS];

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial1.begin(9600);
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

  FastLED.clear();  // initialize in OFF state
  FastLED.show();

//enable the WDT
  int countdownMS = Watchdog.enable(4000);
  Serial.print("Enabled the watchdog with max countdown of ");
  Serial.print(countdownMS, DEC);
  Serial.println(" milliseconds!");
  Serial.println();

  initializeGame();
  testAllTestsFSM();
  runAllHelperFunctionTests();
}

void loop() {
  // static displayState currState = sDisplayGame;
  static bool firstRun = true;

  processMessage();
  switch (currState) {
    case sOFF:
      firstRun = true;
      FastLED.clear();  // turn all LEDs OFF
      FastLED.show();
      // Serial.println("Off State");
      break;
    case sDisplayPatterns:
      firstRun = true;
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
    case sDisplayRainbow:
      firstRun = false;
      rainbowLoop(leds);
    case sDisplayConfetti:
      firstRun = false;
      confettiLoop(leds);
    // default:
    // Serial.println("Invalid/Unexpected State");
  }

  // Petting the watchdog
  Watchdog.reset();
  // Serial.print("Watchdog has been pet!");
  // Serial.println();


}

void processMessage() {
  if (Serial1.available() > 0) {
    // Serial.println(Serial1.readString());
    Serial.println("Read Message");
    String oldMessage = Serial1.readStringUntil('<');
    String message = Serial1.readStringUntil('>');
    message.trim();
    Serial.println(message);
    if (message == "U") {
      lastButtonPressed = UP;
    } else if (message == "R") {
      lastButtonPressed = RIGHT;
    } else if (message == "D") {
      lastButtonPressed = DOWN;
    } else if (message == "L") {
      lastButtonPressed = LEFT;
    } else if (message == "S") {
      Serial.println("changed curr state");
      currState = sDisplayGame;
    } else if (message == "G") {
      currState = sDisplayPatterns;
    } else if (message == "F") {
      currState = sOFF;
    } else if (message == "RN") {
      currState = sDisplayRainbow;
    } else if (message == "CF") {
      currState = sDisplayConfetti;
    } else if (strContains(message, "CB") || strContains(oldMessage, "CB")) {
      Serial.println("contains called");
      Serial1.readStringUntil('\n');
      Serial1.readStringUntil('\n');
      Serial.print("Bytes Available After Clear:");
      Serial.println(Serial1.available());
    } else if (message == "WDT") {
      delay(7000);
    }
  }
}

bool strContains(const String &haystack, const String &needle) {
    return haystack.indexOf(needle) != -1;
}

String trim(const String& str) {
  int startIdx = 0;
  int endIdx = str.length() - 1;

  // Find the start index of the first non-whitespace character
  while (startIdx <= endIdx && isspace(str[startIdx])) {
    startIdx++;
  }

  // Find the end index of the last non-whitespace character
  while (endIdx >= startIdx && isspace(str[endIdx])) {
    endIdx--;
  }

  // Extract the trimmed substring
  return str.substring(startIdx, endIdx + 1);
}
