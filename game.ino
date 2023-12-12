#include "game.h"
#include <deque>
#include <array>

int boardMap[SCI_LI_HEIGHT][SCI_LI_WIDTH];
std::deque<std::array<int, 2>> snakeDeque;
static orientation o;
static int timeStep, savedClock;
static int mils;
static int score;
static int length;
static state CURRENT_STATE;

const int resetButtonPin = 2;

void initializeGame() {
  // testColorPoint(SCI_LI_HEIGHT, 0, CRGB::Green);
  CURRENT_STATE = WAIT_START;
  snakeDeque.clear();
  initializeMap();
  timeStep = 1000;
  mils = millis();
  savedClock = mils;
  pinMode(resetButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(resetButtonPin), resetButtonISR, FALLING);
  
}

void displayGame() {
  // this has to keep track of last button pressed -- should happen in sci-li ino loop instead of an update inputs func 
  // updateInputs();
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), lastButtonPressed);

  for (int i = 0; i < SCI_LI_HEIGHT; i++){
    for (int j = 0; j < SCI_LI_WIDTH; j++){
      if (boardMap[i][j] == FLAG_PLAIN_CELL) {
        testColorPoint(i, j, CRGB:: Black);
      } else if (boardMap[i][j] == FLAG_SNAKE) {
        testColorPoint(i, j, CRGB:: White);
      } else if (boardMap[i][j] == FLAG_FOOD) {
        testColorPoint(i, j, CRGB:: Green);
      } else if (boardMap[i][j] == FLAG_END) {
        testColorPoint(i, j, CRGB::Red);
      }
      // else case?
    }
  }

  FastLED.show();
  delay(10);
}

xy calculateXY(int row, int col) {
  // max_height - row
  uint8_t x = col;
  uint8_t y = SCI_LI_HEIGHT - (row + 1);
  return { x, y };
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
  int snakeRow = 5;
  int snakeCol = 2;
  snakeDeque.push_front({snakeRow, snakeCol});
  // o = (orientation) random(0, 3);
  o = RIGHT;
  length = 1;

  placeFood();
}

// Randomly places food on the board
void placeFood(){
  int food_row = rand() % SCI_LI_HEIGHT;
  int food_col = rand() % SCI_LI_WIDTH;
  // int food_row = 5;
  // int food_col = 3;
  Serial.print("food row:");
  Serial.println(food_row);
  Serial.print("food col:");
  Serial.println(food_col);

  // Checks that the snake is not in the randomly-chosen cell
  if (boardMap[food_row][food_col] == FLAG_PLAIN_CELL) {
    boardMap[food_row][food_col] = FLAG_FOOD;
  } else {
    placeFood(); // Recurse until we find a plain cell
  }
}

// Checks if the snake is facing a wall
bool facingWall(byte o) {
  int currentHeadRow = snakeDeque.front()[0];
  int currentHeadCol = snakeDeque.front()[1];

  switch (o) {
    case UP:
      if (currentHeadRow == 0) {
        return true;
      }
      break;
    case DOWN:
      if (currentHeadRow == SCI_LI_HEIGHT - 1) {
        return true;
      }
      break;
    case RIGHT:
      if (currentHeadCol == SCI_LI_WIDTH - 1) {
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

// Checks if the snake is moving into itself
bool isIntoSelf(byte o) {
  int currentHeadRow = snakeDeque.front()[0];
  int currentHeadCol = snakeDeque.front()[1];

  switch (o) {
    case UP:
      if (boardMap[currentHeadRow - 1][currentHeadCol] == FLAG_SNAKE) {
        return true;
      }
      break;
    case DOWN:
      if (boardMap[currentHeadRow + 1][currentHeadCol] == FLAG_SNAKE) {
        return true;
      }
      break;
    case RIGHT:
      if (boardMap[currentHeadRow][currentHeadCol + 1] == FLAG_SNAKE) {
        return true;
      }
      break;
    case LEFT:
      if (boardMap[currentHeadRow][currentHeadCol - 1] == FLAG_SNAKE) {
        return true;
      }
      break;
  }
  return false;
}

// Check if the input direction is > 90 degrees
bool invalidRotation(byte o, byte lastButton) {
  if ((o == RIGHT and lastButton == LEFT) or (o == LEFT and lastButton == RIGHT)){
    return true;
  }
  else if ((o == UP and lastButton == DOWN) or (o == DOWN and lastButton == UP)){
    return true;
  }
  else {
    return false;
  }
}

// Need to check if moving into a food cell
bool isEating(byte o) {
  int nextHeadRow = snakeDeque.front()[0];
  int nextHeadCol = snakeDeque.front()[1];

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
  // Serial.print("Next Head Row:");
  // Serial.println(nextHeadRow);
  // Serial.print("Next Head Col:");
  // Serial.println(nextHeadCol);

  // Check if the next head position contains food
  if (boardMap[nextHeadRow][nextHeadCol] == FLAG_FOOD) {
    Serial.println("is eating: true");
    return true;
  }
  return false;
}


// Called each time the snake moves
void move(byte o) {
  int currentHeadRow = snakeDeque.front()[0];
  int currentHeadCol = snakeDeque.front()[1];

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

  snakeDeque.push_front({currentHeadRow, currentHeadCol});
  boardMap[currentHeadRow][currentHeadCol] = FLAG_SNAKE;

  // Remove the tail from the boardMap and snakeDeque
  int tailRow = snakeDeque.back()[0];
  int tailCol = snakeDeque.back()[1];
  boardMap[tailRow][tailCol] = FLAG_PLAIN_CELL;
  snakeDeque.pop_back();
}

// Called when the snake is moving and eating
void moveAndEat(byte o) {
  // Save the current head position
  int currentHeadRow = snakeDeque.front()[0];
  int currentHeadCol = snakeDeque.front()[1];

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

  snakeDeque.push_front({currentHeadRow, currentHeadCol});
  boardMap[currentHeadRow][currentHeadCol] = FLAG_SNAKE;

  score++;
  length++;
  placeFood();
}

// Game over function turns the entire board red
void gameOver() {
  for (int i = 0; i < SCI_LI_HEIGHT; i++){
    for (int j = 0; j < SCI_LI_WIDTH; j++){
      boardMap[i][j] = FLAG_END;
    }
  }
}

// Interrupt service routine to reset the game on button press
void resetButtonISR() {
  initializeGame();
}

// Main logic to update FSM state
state updateFSM(state currState, long mils, orientation lastButton) {
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
      nextState = MOV;
      if ((mils - savedClock >= timeStep)) {
        if (!invalidRotation(o, lastButton)) {
          o = lastButton;
        }
        if (!(facingWall(o) or isEating(o) or isIntoSelf(o))) { // transition 2-2
          move(o);
          savedClock = mils;
          nextState = MOV;
        } else if (isEating(o)) { // transition 2-3
          moveAndEat(o);
          savedClock = mils;
          Serial.println("Transition to EATING");
          nextState = EATING;
        } else if (facingWall(o) or isIntoSelf(o)) { // transition 2-4
          gameOver();
          nextState = GAME_OVER;
          Serial.println("Transition to GAME_OVER");
        }
      }
      break;
    case EATING:
      nextState = EATING;
      if (mils - savedClock >= 300) { // transition 3-2
        savedClock = mils;
        nextState = MOV;
        Serial.println("Transition to MOV");
      }
      break;
    case GAME_OVER:
      nextState = GAME_OVER; // transition 4-4
      break;
    default:
      Serial.println("Invalid Case");
  }
  return nextState;
} 

void printOrientation(orientation o) {
  if (o == UP) {
    Serial.println("UP");
  } else if (o == DOWN) {
     Serial.println("DOWN");
  } else if (o == LEFT) {
     Serial.println("LEFT");
  } else if (o == RIGHT) {
     Serial.println("RIGHT");
  }
}