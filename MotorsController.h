#ifndef __MOTORS_CONTROLLER_H
#define __MOTORS_CONTROLLER_H

#include "HBridgeMotor.h"
#include <string>


class MotorsController {
public:
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
    unsigned int getMaxSpeed() const;

private:
    //Constants//
    static const std::string kTag;
    static const unsigned int kDelayMs;
    static const unsigned int kMinMotorSpeed;
    static const unsigned int kMaxMotorSpeed;

    //Variables//
    HBridgeMotor motorRight;
    HBridgeMotor motorLeft;
};

#endif // __MOTORS_CONTROLLER_H