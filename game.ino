#include "game.h"

int boardMap[SCI_LI_HEIGHT][SSCI_LI_WIDTH];

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
    break;
    case MOV:
    break;
    case EATING:
    break;
    case GAME_OVER:
    break;
    case DEFAULT:
    break;
  }
} 
