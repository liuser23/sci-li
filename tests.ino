#include "game.h"
#include <deque>
#include <array>
#include <string>

typedef struct {
  orientation lastButton;
  long mils;
} state_inputs;

typedef struct {
  orientation o;
  int timeStep;
  int savedClock;
  std::deque<std::array<int, 2>> snakeDeque;
} state_vars;


bool testTransition(state startState,
                     state endState,
                     state_inputs testStateInputs, 
                     state_vars startStateVars,
                     state_vars endStateVars,
                     bool verbos);
   

/*        
 * Helper function for printing states
 */
char* s2str(state s) {
  switch(s) {
    case WAIT_START:
    return "(0) WAIT_START";
    case MOV:
    return "(1) MOV";
    case EATING:
    return "(2) EATING";
    case GAME_OVER:
    return "(3) GAME_OVER";
    default:
    return "???";
  }
}


/*
 * Helper function for printing orientations
 */
char* o2str(orientation o) {
  switch(o) {
    case UP:
    return "UP";
    case RIGHT:
    return "RIGHT";
    case DOWN:
    return "DOWN";
    case LEFT:
    return "LEFT";
    default:
    return "???";
  }
}


bool testTransition(state startState,
                    state endState,
                    state_inputs testStateInputs,
                    state_vars startStateVars,
                    state_vars endStateVars,
                    bool verbos) {
  // Initialize variables to store state changes
  o = startStateVars.o;
  timeStep = startStateVars.timeStep;
  savedClock = startStateVars.savedClock;
  snakeDeque = startStateVars.snakeDeque;  // Save the initial state of snakeDeque

  // Call the updateFSM function
  state resultState = updateFSM(startState, testStateInputs.mils, testStateInputs.lastButton);

  // Compare the state changes
  bool passedTest = (endState == resultState &&
                     o == endStateVars.o &&
                     timeStep== endStateVars.timeStep &&
                     savedClock == endStateVars.savedClock &&
                     snakeDeque == endStateVars.snakeDeque);
                  
  // Print details if verbose mode is enabled
  if (!verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    Serial.println(s2str(startState));
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "Inputs: mils %ld | lastButton %s", testStateInputs.mils, o2str(testStateInputs.lastButton));
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %5s | %9s | %11s", "o", "timeStep", "savedClock");
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %5s | %9d | %11d", o2str(endStateVars.o), endStateVars.timeStep, endStateVars.savedClock);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %5s | %9d | %11d", o2str(o), timeStep, savedClock);
    Serial.println(sToPrint);
    return false;
  }
}
std::deque<std::array<int, 2>> convertToDeque(const std::array<int, 2> arr[], std::size_t size) {
    std::deque<std::array<int, 2>> resultDeque;

    for (std::size_t i = 0; i < size; ++i) {
        resultDeque.push_back(arr[i]);
    }

    return resultDeque;
}



//different sets of snake coordinates to test correct movement, growing after eating, and running into a wall 
std::array<int, 2> coordinatesArray[] = {{1, 2}};
std::array<int, 2> coordinatesArrayUp[] = {{0, 2}};
std::array<int, 2> coordinatesArrayUpAfterEat[] = {{0, 2}, {1,2}};
std::array<int, 2> coordinatesArrayAfterEat[] = {{1, 3}, {1, 2}};
std::array<int, 2> coordinatesArrayFacingWall[] = {{0, 0}};


std::deque<std::array<int, 2>> testSnakeDeque = convertToDeque(coordinatesArray, sizeof(coordinatesArray) / sizeof(coordinatesArray[0]));
std::deque<std::array<int, 2>> snakeDequeUp = convertToDeque(coordinatesArrayUp, sizeof(coordinatesArrayUp) / sizeof(coordinatesArrayUp[0]));
std::deque<std::array<int, 2>> snakeDequeUpAfterEat = convertToDeque(coordinatesArrayUp, sizeof(coordinatesArrayUp) / sizeof(coordinatesArrayUp[0]));
std::deque<std::array<int, 2>> snakeDequeAfterEat = convertToDeque(coordinatesArrayAfterEat, sizeof(coordinatesArrayAfterEat) / sizeof(coordinatesArrayAfterEat[0]));
std::deque<std::array<int, 2>> snakeDequeFacingWall = convertToDeque(coordinatesArrayFacingWall, sizeof(coordinatesArrayFacingWall) / sizeof(coordinatesArrayFacingWall[0]));


/*
*Test all the state transitions: 
*0-0: (WAIT_START- WAIT-START) milis - savedClock < timeStep so the transition to state 1 is not taken
*0-1: (WAIT_START to MOV) the transition is taken because enough time has passed
*1-1 (MOV to MOV) milis - savedClock < timeStep so still in same state/ no transition
*1-1 (MOV to MOV) snake does not run into itself, food, or wall so it continues to be in MOV step
*1-2 (MOV to EAT) snake moves into a cell with FLAG_FOOD so the snake should grow by not clearing the tail cell
*1-3 (MOV to GAME_OVER) snake moves into a wall so enters GAME_OVER state
*2-1 (EAT to MOV) snake will continue moving after 300 ms delay in the eat step
*2-2 (EAT to EAT) not enough time has passed so still in EAT step
*3-3 (GAME_OVER to GAME_OVER) game has not been reset- still in GAME_OVER state
*/
const state testStatesIn[9] = { (state) 0, (state) 0, (state) 1, (state) 1, (state) 1, (state) 1, (state) 2, (state) 2, (state) 3};

const state testStatesOut[9] = {(state) 0, (state) 1, (state) 1, (state) 1, (state) 2, (state) 3, (state) 1, (state) 2, (state) 3};

const state_inputs testInputs[9] = {{DOWN, 500}, {DOWN, 700}, {LEFT, 500}, {UP, 1200}, {RIGHT,900}, {LEFT, 600}, {UP, 900}, {UP, 400}, {UP, 400}};

const state_vars testVarsIn[9] = {{DOWN, 600, 0, testSnakeDeque}, {DOWN, 600, 0, testSnakeDeque}, {LEFT, 600, 0, testSnakeDeque}, {RIGHT, 600, 0, testSnakeDeque}, {RIGHT, 600, 0, testSnakeDeque}, {LEFT, 600, 0, snakeDequeFacingWall}, {UP, 600, 300, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}};

const state_vars testVarsOut[9] = {{DOWN, 600, 0, testSnakeDeque}, {DOWN, 600, 700, testSnakeDeque}, {LEFT, 600, 0, testSnakeDeque}, {UP, 600, 1200, snakeDequeUp}, {RIGHT, 600, 900, snakeDequeAfterEat}, {LEFT, 600, 0, snakeDequeFacingWall}, {UP, 600, 900, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}};

const int numTests = 9;

/*
 * Unit test for isEating helper function 
 */
void isEatingTests() {
    Serial.println("isEatingTests");
    // Scenario 1: Snake is about to eat (next head position has food)
    snakeDeque.clear();  // Clear the deque
    snakeDeque.push_front({1, 1});  // Set the current head position
    boardMap[0][1] = FLAG_FOOD;     // Set food at the next head position
    Serial.println("Scenario 1:");
    Serial.print("Expected: 1 | Actual: ");
    Serial.println((bool)  isEating(UP));  // Snake is about to eat

    // Scenario 2: Snake is not about to eat (next head position doesn't have food)
    snakeDeque.clear();  // Clear the deque
    snakeDeque.push_front({2, 2});  // Set the current head position
    boardMap[1][2] = FLAG_PLAIN_CELL;  // No food at the next head position
    Serial.println("Scenario 2:");
    Serial.print("Expected: 0 | Actual: ");
    Serial.println((bool) isEating(DOWN));  // Snake is not about to eat

    Serial.println("----------------");
}

/*
 * Unit test for invalidRotation helper function 
 */
void invalidRotationTests() {
    Serial.println("invalidRotationTests");
    // Scenario 1: Invalid rotation (180 degrees)
    Serial.println("Scenario 1:");
    Serial.print("Expected: 1 | Actual: ");
    Serial.println(invalidRotation(UP, DOWN));  // Invalid rotation

    // Scenario 2: Valid rotation (not 180 degrees)
    Serial.println("Scenario 2:");
    Serial.print("Expected: 0 | Actual: ");
    Serial.println(invalidRotation(UP, RIGHT));  // Valid rotation

    Serial.println("----------------");
}

/*
 * Unit test for isIntoSelf helper function 
 */
void isIntoSelfTest() {
    snakeDeque.clear();
    //snake of size 4 that could run into itself
    snakeDeque.push_front({2, 2});  
    snakeDeque.push_front({2, 3}); 
    snakeDeque.push_front({3, 3}); 
    snakeDeque.push_front({3, 2}); 

    // Set up the board scenario
    boardMap[2][2] = FLAG_SNAKE; 
    boardMap[2][3] = FLAG_SNAKE;  
    boardMap[3][2] = FLAG_SNAKE;  
    boardMap[3][3] = FLAG_SNAKE;  


    // Scenario 1: Move the snake into itself
    Serial.println("Scenario 1:");
    Serial.print("Expected: 1 | Actual: ");
    Serial.println(isIntoSelf(UP));

    // Scenario 1: Snake does not move into intself
    Serial.println("Scenario 1:");
    Serial.print("Expected: 0 | Actual: ");
    Serial.println(isIntoSelf(DOWN));

    Serial.println("----------------");
}

/*
 * Unit test for facingWallTest helper function 
 */
void facingWallTest() {
    Serial.println("facingWallTest");
    snakeDeque.clear();
    snakeDeque.push_front({0, 3});  

    // Scenario 1: Snake is facing a wall (UP)
    Serial.println("Scenario 1:");
    Serial.print("Expected: 1 | Actual: ");
    Serial.println(facingWall(UP)); 

    // Scenario 2: Snake is not facing a wall (RIGHT)
    Serial.println("Scenario 2:");
    Serial.print("Expected: 0 | Actual: ");
    Serial.println(facingWall(RIGHT)); 

    Serial.println("----------------");
}

/*
 * Unit test for move helper function 
 */
void moveTest() {
    Serial.println("moveTest");
    snakeDeque.clear();
    snakeDeque.push_front({0, 3}); 

    move(LEFT);
    int currentHeadRow = snakeDeque.front()[0];
    int currentHeadCol = snakeDeque.front()[1];
    

    //the expected snake deque after moving left once
    std::array<int, 2> coordinatesArrayLeft[] = {{0, 2}};
    std::deque<std::array<int, 2>> snakeDequeLeft = convertToDeque(coordinatesArrayLeft, sizeof(coordinatesArrayLeft) / sizeof(coordinatesArrayLeft[0]));
 
    if (snakeDeque == snakeDequeLeft && boardMap[currentHeadRow][currentHeadCol] == FLAG_SNAKE){
      Serial.println("Move test passed! The snake moved properly.");
    }
    else{
      Serial.println("Move test failed. The snake did not move properly.");
    }
    Serial.println("----------------");
}

 /*
 * Unit test for moveAndEat helper function 
 */
void moveAndEatTest() {
    Serial.println("moveAndEatTest");
    snakeDeque.clear();
    snakeDeque.push_front({0, 3});
    int currentHeadRow = snakeDeque.front()[0];
    int currentHeadCol = snakeDeque.front()[1];
    boardMap[currentHeadRow][currentHeadCol] = FLAG_SNAKE;

    moveAndEat(LEFT);
    currentHeadRow = snakeDeque.front()[0];
    currentHeadCol = snakeDeque.front()[1];
  
    //the expected snake deque after eating (and moving left)
    std::array<int, 2> coordinatesArrayAfterEat[] = {{0, 2}, {0, 3}}; //after eating it should keep the tail to show the snake growing
    std::deque<std::array<int, 2>> snakeDequeAfterEat = convertToDeque(coordinatesArrayAfterEat, sizeof(coordinatesArrayAfterEat) / sizeof(coordinatesArrayAfterEat[0]));    

    if (snakeDeque == snakeDequeAfterEat && boardMap[currentHeadRow][currentHeadCol] == FLAG_SNAKE && boardMap[currentHeadRow][currentHeadCol+1] == FLAG_SNAKE){ //snake grows so head and tail are still flagged as snake cells
      Serial.println("Move and eat test passed! The snake moved and grew properly after eating.");
    }
    else{
      Serial.println("Move and eat test failed. The snake did not move and eat properly.");
    }

    Serial.println("----------------");
}


 
bool testAllTestsFSM() {
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All FSM tests passed!");
  return true;
}

void runAllHelperFunctionTests(){
  facingWallTest();
  isIntoSelfTest();
  invalidRotationTests();
  isEatingTests();
  moveTest();
  moveAndEatTest();
}

