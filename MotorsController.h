#ifndef __MOTORS_CONTROLLER_H
#define __MOTORS_CONTROLLER_H

#include "HBridgeMotor.h"
#include <stdint.h>
#include <string>


class MotorsController {
public:
    MotorsController(
        uint8_t enR, 
        uint8_t in1R, 
        uint8_t in2R, 
        uint8_t enL, 
        uint8_t in1L, 
        uint8_t in2L
    );
    void init();
    void updateSpeed(uint8_t speedR, uint8_t speedL);
    void updateDirection(HBridgeMotor::Direction directionR, HBridgeMotor::Direction directionL);
    uint8_t getMaxSpeed();


private:
    //Constants//
    static const std::string kTag;
    static const uint32_t kDelayMs;
    static const uint8_t kMaxMotorSpeed;
    static const uint8_t kMinMotorSpeed;

    //Variables//
    HBridgeMotor motorRight;
    HBridgeMotor motorLeft;
};

#endif // __MOTORS_CONTROLLER_H