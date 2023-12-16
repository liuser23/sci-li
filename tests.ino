#include "game.h"

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


std::array<int, 2> coordinatesArray[] = {{1, 2}};
std::array<int, 2> coordinatesArrayUp[] = {{2, 2}};
std::array<int, 2> coordinatesArrayAfterEat[] = {{2, 1}, {2, 2}};
std::array<int, 2> coordinatesArrayFacingWall[] = {{0, 0}};



std::deque<std::array<int, 2>> testSnakeDeque = convertToDeque(coordinatesArray, sizeof(coordinatesArray) / sizeof(coordinatesArray[0]));
std::deque<std::array<int, 2>> snakeDequeUp = convertToDeque(coordinatesArrayUp, sizeof(coordinatesArrayUp) / sizeof(coordinatesArrayUp[0]));
std::deque<std::array<int, 2>> snakeDequeAfterEat = convertToDeque(coordinatesArrayAfterEat, sizeof(coordinatesArrayAfterEat) / sizeof(coordinatesArrayAfterEat[0]));
std::deque<std::array<int, 2>> snakeDequeFacingWall = convertToDeque(coordinatesArrayFacingWall, sizeof(coordinatesArrayFacingWall) / sizeof(coordinatesArrayFacingWall[0]));



const state testStatesIn[9] = {(state) 0, (state) 0, (state) 1, (state) 1, (state) 1, (state) 1, (state) 2, (state) 2, (state) 3};

const state testStatesOut[9] = {(state) 0, (state) 1, (state) 1, (state) 1, (state) 2, (state) 3, (state) 1, (state) 2, (state) 3};

const state_inputs testInputs[9] = {{DOWN, 500}, {DOWN, 700}, {LEFT, 500}, {UP, 1200}, {RIGHT, 600}, {LEFT, 600}, {UP, 600}, {UP, 400}, {UP, 400}};

const state_vars testVarsIn[9] = {{UP, 600, 0, testSnakeDeque}, {DOWN, 600, 0, testSnakeDeque}, {LEFT, 600, 0, testSnakeDeque}, {RIGHT, 600, 0, testSnakeDeque}, {RIGHT, 600, 0, testSnakeDeque}, {LEFT, 600, 0, snakeDequeFacingWall}, {UP, 600, 300, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}};

const state_vars testVarsOut[9] = {{UP, 600, 0, testSnakeDeque}, {DOWN, 600, 700, testSnakeDeque}, {LEFT, 600, 0, testSnakeDeque}, {UP, 600, 600, snakeDequeUp}, {RIGHT, 600, 600, snakeDequeAfterEat}, {LEFT, 600, 600, snakeDequeFacingWall}, {UP, 600, 1000, snakeDequeUp}, {UP, 600, 400, testSnakeDeque}, {UP, 600, 300, testSnakeDeque}};

const int numTests = 9;



 
bool testAllTests() {
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}

