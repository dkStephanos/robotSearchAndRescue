/**
 * ---------------------------------------------------------------------------
 * File name: Robot.cpp
 * Project name: Project 5
 * Purpose: Provides the functionality of the Robot class
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 3/8/2019
 * ---------------------------------------------------------------------------
*/
#include "Robot.h"
#include "Board.h"

Robot::Robot() {
  position[0] = 0;
  position[1] = 0;
}

Robot::Robot(Board curr_board) {
  position[0] = 0;
  position[1] = 0;
  board = curr_board;
}

void Robot::setPosition(int curr_position[2]) {
  position[0] = curr_position[0];
  position[1] = curr_position[1];
}

void Robot::setBoard(Board curr_board) {
    board = curr_board;
}

int * Robot::getPosition() {
  return this->position;
}

Board Robot::getBoard() {
  return this->board;
}

int Robot::updatePosition(char direction) {
  int distance = 1;      //Can be moved to parameter later on to allow for moves of multiple lengths.
  switch(direction) {
    case 'E':
      if(position[0] + distance < board.width) {  //if we can move the whole distance, do it
        position[0] += distance;
      } else if(position[0] < board.width - 1) { //otherwise, provided we aren't already at the edge, move there
        position[0] = board.width - 1;
      }
      break;
    case 'W':
      if(position[0] - distance >= 0) {  //if we can move the whole distance, do it
        position[0] -= distance;
      } else if(position[0] > 0) { //otherwise, provided we aren't already at the edge, move there
        position[0] = 0;
      }
      break;
    case 'N':
      if(position[1] + distance < board.height) {  //if we can move the whole distance, do it
        position[1] += distance;
      } else if(position[1] < board.height - 1) { //otherwise, provided we aren't already at the edge, move there
        position[1] = board.height - 1;
      }
      break;
    case 'S':
      if(position[1] - distance >= 0) {  //if we can move the whole distance, do it
        position[1] -= distance;
      } else if(position[1] > 1) { //otherwise, provided we aren't already at the edge, move there
        position[1] = 0;
      }
      break;
    default:
      return -1;  //if the direction doesn't match, we have a bad command or we are quitting
  }
  return 1; //Signals everything went okay
}
