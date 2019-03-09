#ifndef ROBOT_H
#define ROBOT_H
/**
 * ---------------------------------------------------------------------------
 * File name: Robot.h
 * Project name: Project 3
 * Purpose: Outline of functionality for the Robot class
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 3/8/2019
 * ---------------------------------------------------------------------------
*/
#include "Board.h"
#include <string>

class Robot {
private:
    int position [2];
    Board board;

    //Updates the position of the robot
    void setPosition(int curr_position[2]);
public:
    // Constructors
    // Default constructor: sets position to 0,0
    Robot( );

    //Constructor that takes in a current board
    Robot(Board curr_board);

    //Returns current position of robot as a pointer to an array containing (x,y)
    int * getPosition();

    //Returns current board
    Board getBoard();

    //Moves the robot the specified distance in the specified direction by
    //utilizing the private setPosition method.
    int updatePosition(char direction);

}; // class Robot

#endif
