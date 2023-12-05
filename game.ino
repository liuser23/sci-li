#include "game.h"


int boardMap[SCI_LI_HEIGHT][SSCI_LI_WIDTH];

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

//check if the input direction > 90 degrees
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




void initializeMap(){
  for (int i = 0; i < SCI_LI_HEIGHT){
    for (int j = -; j < SCI_LI_WIDTH){
      boardMap[i][j] = FLAG_PLAIN_CELL;
    }
  }
  int snake_row = rand() % SCI_LI_HEIGHT;
  int snake_col = rand() % SCI_LI_WIDTH;

  boardMap[snake_row][snake_col] = FLAG_SNAKE;

  int exampleSnakeCoords[1][2] = {
    { snake_row, snake_col}

  };
  exampleColorSnake(exampleSnakeCoord, 1)

  placeFood();
}

void placeFood(){
  int food_row = rand() % SCI_LI_HEIGHT;
  int food_col = rand() % SCI_LI_WIDTH;

  if boardMap[food_row][food_col] == FLAG_PLAIN_CELL{
    boardMap[food_row][food_col] = FLAG_FOOD;
    leds[ledLocation.x][ledLocation.y] = CRGB::Green;

  } else {
    placeFood();
  }
}
