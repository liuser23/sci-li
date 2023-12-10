#include "game.h"

int boardMap[SCI_LI_HEIGHT][SCI_LI_WIDTH];
int snakeCoords[0][2];
static orientation o;
static int timeStep, savedClock;
static int mils;
static int score;
static int length;



void initializeGame() {
  initializeMap();
  timeStep = 1000;
  mils = millis();
  savedClock = mils;
}

void displayGame() {
  static state CURRENT_STATE = WAIT_START;
  // Not sure what this does! Probably checks for button inputs from lab
  // updateInputs();
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), numButtonsPressed, lastButtonPressed);
  FastLED.show();
  delay(10);
}

xy calculateXY(int row, int col) {
  // max_height - row
  uint8_t x = col;
  uint8_t y = 14 - row;
  return { x, y };
}

void exampleColorSnake(int snakeCoords[][2], size_t snakeSize) {
  for (size_t i = 0; i < snakeSize; i++) {
    testColorPoint(snakeCoords[i][0], snakeCoords[i][1], CRGB::White);
  }
}


void testColorPoint(int row, int col, CRGB color) {
  xy ledLocation = calculateXY(row, col);
  leds[ledLocation.x][ledLocation.y] = color;
}

// Initializes the game board, or "map"
void initializeMap(){

  // Sets all cells on the board to be plain cells
  for (int i = 0; i < SCI_LI_HEIGHT; i++){
    for (int j = 0; j < SCI_LI_WIDTH; j++){
      boardMap[i][j] = FLAG_PLAIN_CELL;
    }
  }

  score = 0;

  // Places the snake at a random point on the board
  boardMap[5][2] = FLAG_SNAKE;
  o = (orientation) random(0, 3);
  length = 1;
  testColorPoint(5, 2, CRGB::White);

  placeFood();
}

// Randomly places food on the board
void placeFood(){
  int food_row = rand() % SCI_LI_HEIGHT;
  int food_col = rand() % SCI_LI_WIDTH;

  // Checks that the snake is not in the randomly-chosen cell
  if (boardMap[food_row][food_col] == FLAG_PLAIN_CELL) {
    boardMap[food_row][food_col] = FLAG_FOOD;
    testColorPoint(food_row, food_col, CRGB::Green);
  } else {
    placeFood(); // Recurse until we find a plain cell
  }
}

// Checks if the snake is facing a wall
bool facingWall(byte o) {
  int currentHeadRow = snakeCoords[0][0];
  int currentHeadCol = snakeCoords[0][1];
  switch (o) {
    case UP:
      if (currentHeadRow == 0) {
        return true;
      }
      break;
    case DOWN:
      if (currentHeadRow == SCI_LI_HEIGHT) {
        return true;
      }
      break;
    case RIGHT:
      if (currentHeadCol == SCI_LI_WIDTH) {
        return true;
      }
      break;
    case LEFT:
      if (currentHeadCol == 0) {
        return true;
      }
      break;
  }
  return false;
}

// Check if the input direction is > 90 degrees
bool isIntoSelf(byte o, int lastButton) {
  if ((o == RIGHT and lastButton == 2) or (o == LEFT and lastButton == 3)){
    return true;
  }
  else if ((o == UP and lastButton == 1) or (o == DOWN and lastButton == 0)){
    return true;
  }
  else {
    return false;
  }
}

// Need to check if moving into a food cell
bool isEating(byte o, int lastButton) {
  int nextHeadRow = snakeCoords[0][0];
  int nextHeadCol = snakeCoords[0][1];

  // Calculate the next head position based on the current orientation
  switch (o) {
    case UP:
      nextHeadRow--;
      break;
    case DOWN:
      nextHeadRow++;
      break;
    case RIGHT:
      nextHeadCol++;
      break;
    case LEFT:
      nextHeadCol--;
      break;
  }

  // Check if the next head position contains food
  if (boardMap[nextHeadRow][nextHeadCol] == FLAG_FOOD) {
    return true;
  }

  return false;
}


// Called each time the snake moves
void move(byte o) {
  // Save the current head position
  int currentHeadRow = snakeCoords[0][0];
  int currentHeadCol = snakeCoords[0][1];
  Serial.print("Current Head Row:");
  Serial.println(currentHeadRow);
  Serial.print("Current Head Col:");
  Serial.println(currentHeadCol);

  // Update the head position based on the current orientation
  switch (o) {
    case UP:
      currentHeadRow--;
      break;
    case DOWN:
      currentHeadRow++;
      break;
    case RIGHT:
      currentHeadCol++;
      break;
    case LEFT:
      currentHeadCol--;
      break;
  }

  boardMap[currentHeadRow][currentHeadCol] = FLAG_SNAKE;
  testColorPoint(currentHeadRow, currentHeadCol, CRGB::White);
  snakeCoords[0][0] = currentHeadRow;
  snakeCoords[0][1] = currentHeadCol;

  // Remove the tail from the boardMap and snakeCoords
  int tailRow = snakeCoords[length - 1][0];
  int tailCol = snakeCoords[length - 1][1];
  boardMap[tailRow][tailCol] = FLAG_PLAIN_CELL;
  testColorPoint(tailRow, tailCol, CRGB::Black);
  // Update the rest of the snakeCoords array by shifting elements
  for (int i = length - 1; i > 0; i--) {
    snakeCoords[i][0] = snakeCoords[i - 1][0];
    snakeCoords[i][1] = snakeCoords[i - 1][1];
  }
}

// Called when the snake is moving and eating
void moveAndEat(byte o) {
  // Save the current head position
  int currentHeadRow = snakeCoords[0][0];
  int currentHeadCol = snakeCoords[0][1];

  // Update the head position based on the current orientation
  switch (o) {
    case UP:
      currentHeadRow--;
      break;
    case DOWN:
      currentHeadRow++;
      break;
    case RIGHT:
      currentHeadCol++;
      break;
    case LEFT:
      currentHeadCol--;
      break;
  }

  boardMap[currentHeadRow][currentHeadCol] = FLAG_SNAKE;
  testColorPoint(currentHeadRow, currentHeadCol, CRGB::White);
  snakeCoords[0][0] = currentHeadRow;
  snakeCoords[0][1] = currentHeadCol;

  score++;
  length++;
  placeFood();
}

// Game over function turns the entire board red
void gameOver() {
  for (int i = 0; i < SCI_LI_HEIGHT; i++){
    for (int j = 0; j < SCI_LI_WIDTH; j++){
      testColorPoint(i, j, CRGB::Red);
    }
  }
}

// Main logic to update FSM state
state updateFSM(state currState, long mils, int numButtons, int lastButton) {
  state nextState;
  mils = millis();
  switch(currState) {
    case WAIT_START:
      Serial.println("IN WAIT_START");
      nextState = WAIT_START;
      if (mils - savedClock >= timeStep) { // transition 1-2
        savedClock = mils;
        nextState = MOV;
        Serial.println("Transition to MOV");
      }
      break;
    case MOV:
      Serial.println("In MOV");
      nextState = MOV;
      if ((mils - savedClock >= timeStep) and !(facingWall(o) or isEating(o, lastButton) or isIntoSelf(o, lastButton))) { // NOTE FROM ALANA: idk what to pass into facingWall, transition 2-2
        move(o);
        savedClock = mils;
        nextState = MOV;
      }
      else if ((mils - savedClock >= timeStep) and isEating(o, lastButton)) { // transition 2-3
        moveAndEat(o);
        savedClock = mils;
        Serial.println("Transition to EATING");
        nextState = EATING;
      }
      else if ((mils - savedClock >= timeStep) and (facingWall(o) or isIntoSelf(o, lastButton))) { // transition 2-4
        gameOver();
        nextState = GAME_OVER;
        Serial.println("Transition to GAME_OVER");
      }
      break;
    case EATING:
      Serial.println("In EATING");
      nextState = EATING;
      if (mils - savedClock >= 500) { // transition 3-2
        move(o);
        Serial.println("Transition to MOV");
        nextState = MOV;
      }
      break;
    case GAME_OVER:
      // if () { // trigger on reset button interrupt while in game_over state, transition 4-1
        // initializeMap();
      // }
      Serial.println("In GAME_OVER");
      nextState = GAME_OVER;
      break;
    default:
      // NOTE FROM ALANA: what do we want to do here??
      Serial.println("Invalid Case");
  }
  return nextState;
} 
