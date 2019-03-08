#ifndef LOG_H
#define LOG_H
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

class Robot {
private:
    static int postion [2];
    static Board board;

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

    //Moves the robot the specified distance in the specified direction by
    //utilizing the private setPosition method.
    int updatePosition(int distance, char direction);
    
}; // class Robot

#endif
