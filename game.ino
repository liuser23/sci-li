#include "game.h"

int boardMap[SCI_LI_HEIGHT][SCI_LI_WIDTH];
int snakeCoords[][];
static byte x, y;
static orientation o;
static int timeStep, savedClock;
static int mils;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  initializeSystem();

  /*
   * Initialize all FSM variables
   */
  timeStep = 1000;
  mils = millis();
  savedClock = mils;

  // display something on the LCD
  displayLevel(level, countdown);
}

void loop() {
  static state CURRENT_STATE = sDISP_COUNTDOWN;
  updateInputs();
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), numButtonsPressed, lastButtonPressed);
  delay(10);
}


void testColorPoint(int row, int col, CRGB color) {
  xy ledLocation = calculateXY(row, col);
  leds[ledLocation.x][ledLocation.y] = color;
}

// Initializes the game board, or "map"
void initializeMap(){

  // Sets all cells on the board to be plain cells
  for (int i = 0; i < SCI_LI_HEIGHT){
    for (int j = -; j < SCI_LI_WIDTH){
      boardMap[i][j] = FLAG_PLAIN_CELL;
    }
  }

  int snake_row = rand() % SCI_LI_HEIGHT;
  int snake_col = rand() % SCI_LI_WIDTH;

  // Places the snake at a random point on the board
  boardMap[snake_row][snake_col] = FLAG_SNAKE;
  testColorPoint(snake_row, snake_col, CRGB::White);

  // int exampleSnakeCoords[1][2] = {
  //   { snake_row, snake_col}
  // };
  // exampleColorSnake(exampleSnakeCoord, 1)

  placeFood();
}

// Randomly places food on the board
void placeFood(){
  int food_row = rand() % SCI_LI_HEIGHT;
  int food_col = rand() % SCI_LI_WIDTH;

  // Checks that the snake is not in the randomly-chosen cell
  if boardMap[food_row][food_col] == FLAG_PLAIN_CELL{
    boardMap[food_row][food_col] = FLAG_FOOD;
    testColorPoint(snake_row, snake_col, CRGB::Green);

  } else {
    placeFood(); // Recurse until we find a plain cell
  }
}

// Checks if the snake is facing a wall
bool facingWall(byte x, byte y, byte o, int lxb, int uxb) {
  switch (o) {
    case UP:
      if (y == 0) {
        return true;
      }
      break;
    case DOWN:
      if (y == SCI_LI_HEIGHT) {
        return true;
      }
      break;
    case RIGHT:
      if (x == SCI_LI_WIDTH) {
        return true;
      }
      break;
    case LEFT:
      if (x == 0) {
        return true;
      }
      break;
  }
  return false;
}

// Check if the input direction is > 90 degrees
bool isIntoSelf(byte o, int lastButton) {
  if ((o == RIGHT and lastButton == 2) or o == LEFT and lastButton == 3)){
    return true;
  }
  else if ((o == UP and lastButton == 1) or o == DOWN and lastButton == 0)){
    return true;
  }
  else {
    return false;
  }
}

// Need to check if moving into a food cell
bool isEating(byte o, int lastButton) {
  if (o == RIGHT and snakeCoords)
}


// Called each time the snake moves
void move() {

}

// Called when the snake is moving and eating
void moveAndEat() {

}

// Game over function turns the entire board red
void gameOver() {
  for (int i = 0; i < SCI_LI_HEIGHT){
    for (int j = -; j < SCI_LI_WIDTH){
      testColorPoint(i, j, CRGB::Red);
    }
  }
}

// Main logic to update FSM state
state updateFSM(state currState, long mils, int numButtons, int lastButton) {
  state nextState;
  mils = millis();
  switch(curState) {
    case WAIT_START:
      if (mils - savedClock >= timeStep) { // transition 1-2
        initializeMap();
        savedClock = mils;
        nextState = MOV;
      }
      break;
    case MOV:
      if ((mils - savedClock >= timeStep) and !(facingWall() or isEating()) { // NOTE FROM ALANA: idk what to pass into facingWall, transition 2-2
        move();
        savedClock = mils;
        nextState = MOV;
      }
      else if ((mils - savedClock >= timeStep) and isEating()) { // transition 2-3
        moveAndEat();
        savedClock = mils;
        nextState = EATING;
      }
      else if ((mils - savedClock >= timeStep) and facingWall()) { // transition 2-4
        gameOver();
        nextState = GAME_OVER;
      }
      break;
    case EATING:
      if (mils - savedClock >= 500) { // transition 3-2
        updateGame();
        nextState = MOV;
      }
      break;
    case GAME_OVER:
      if () { // trigger on reset button interrupt while in game_over state, transition 4-1
        initializeMap();
      }
      break;
    case DEFAULT: // NOTE FROM ALANA: what do we want to do here??
      break;
  }
} 
