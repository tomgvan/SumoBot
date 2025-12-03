#ifndef __MOTORS_CONTROLLER_H
#define __MOTORS_CONTROLLER_H

#include <string>
#include "HBridgeMotor.h"


class MotorsController {
public:
    //Methods//
    MotorsController(
        unsigned int enR, 
        unsigned int in1R, 
        unsigned int in2R, 
        unsigned int enL, 
        unsigned int in1L, 
        unsigned int in2L
    );
    void init();
    void updateSpeed(unsigned int speedR, unsigned int speedL);
    void updateDirection(HBridgeMotor::Direction directionR, HBridgeMotor::Direction directionL);

private:
    //Constants//
    static const std::string kTag;

    //Variables//
    HBridgeMotor motorRight;
    HBridgeMotor motorLeft;
};

#endif // __MOTORS_CONTROLLER_H