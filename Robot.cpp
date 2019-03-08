/**
 * ---------------------------------------------------------------------------
 * File name: Robot.cpp
 * Project name: Project 3
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
  position = {0,0};
}

Robot::Robot(Board curr_board) {
  position = {0,0};
  board = curr_board;
}

void Robot::setPosition(int curr_position[2]) {
  position = curr_position;
}

int * Robot::getPosition() {
  return position;
}

int updatePosition(int distance, char direction) {
  switch(direction) {
    case 'N':
      if(position[0] + distance < board.X) {  //if we can move the whole distance, do it
        position[0] += distance;
      } else if(position[0] < board.X - 1) { //otherwise, provided we aren't already at the edge, move there
        position[0] = board.X - 1;
      }
      break;
    case 'S':
      if(position[0] - distance >= 0) {  //if we can move the whole distance, do it
        position[0] -= distance;
      } else if(position[0] > 0) { //otherwise, provided we aren't already at the edge, move there
        position[0] = 0;
      }
      break;
    case 'E':
      if(position[1] + distance < board.Y) {  //if we can move the whole distance, do it
        position[1] += distance;
      } else if(position[1] < board.Y - 1) { //otherwise, provided we aren't already at the edge, move there
        position[1] = board.Y - 1;
      }
      break;
    case 'W':
      if(position[1] - distance >= 0) {  //if we can move the whole distance, do it
        position[1] -= distance;
      } else if(position[1] > 1) { //otherwise, provided we aren't already at the edge, move there
        position[1] = 0;
      }
      break;
    default:
      return -1;  //if the direction doesn't match, we have a bad command
  }
  return 1; //Signals everything went okay
}
