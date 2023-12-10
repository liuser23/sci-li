#ifndef GAME_H
#define GAME_H
#include <FastLED.h>


// Height & Width of Grid - 0-indexed
#define SCI_LI_WIDTH 4
#define SCI_LI_HEIGHT 10

// Cols Values Left-Right (Grid is Currently 15x5 but Some Lights Might Not Be Visible)
#define PIN_COL0 6
#define PIN_COL1 7
#define PIN_COL2 5
#define PIN_COL3 3
#define PIN_COL4 0


//Bitflags to store cell data as integers
#define FLAG_PLAIN_CELL 0b001
#define FLAG_SNAKE 0b010
#define FLAG_FOOD 0b100

//enum for directions
typedef enum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
} orientation;

typedef enum {
 WAIT_START = 0,
 MOV = 1,
 EATING = 2,
 GAME_OVER = 3,
} state;

// idea taken from lab6, prob other useful structs there
// prob will later move these into game.ino/game.h
typedef struct {
  uint8_t x;
  uint8_t y;
} xy;

// variables for keeping track of last direction input
orientation lastButtonPressed;
int numButtonsPressed;

void initializeGame();
void displayGame();

#endif // GAME_H