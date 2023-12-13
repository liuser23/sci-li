#include "animations.h"
#include "game.h"
#include <Arduino.h>


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
  // Clear and enable WDT
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn); 

  // TODO: Configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  // set GCLK->GENCTRL.reg and GCLK->CLKCTRL.reg;
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(0x03);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(0x03);

  // TODO: Configure and enable WDT:
  // use WDT->CONFIG.reg, WDT->EWCTRL.reg, WDT->CTRL.reg
  WDT->CONFIG.reg = WDT_CONFIG_PER(9);
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET_8_Val;
  WDT->CTRL.reg = WDT_CTRL_ENABLE;
  while(WDT->STATUS.bit.SYNCBUSY);
 
  // TODO: Enable early warning interrupts on WDT:
  // reference WDT registers with WDT->registername.reg
  WDT->INTENSET.reg = WDT_INTENSET_EW;
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

  // Petting the wathcdog
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR(0xA5);
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
    } else if (message = "WDT") {
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

void WDT_Handler() {
  // TODO: Clear interrupt register flag
  // (reference register with WDT->registername.reg)
  WDT->INTFLAG.reg |= WDT_INTFLAG_EW;
  
  // TODO: Warn user that a watchdog reset may happen
  // Serial.println("Watchdog timer reset may happen");
}