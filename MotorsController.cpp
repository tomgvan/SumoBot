#include "MotorsController.h"


// Static Constants Initialization
const std::string MotorsController::kTag = "Motors Controller";
const unsigned int MotorsController::kDelayMs = 5;
const unsigned int MotorsController::kMaxMotorSpeed = 255;
const unsigned int MotorsController::kMinMotorSpeed = 0;


MotorsController::MotorsController(
    unsigned int enR, 
    unsigned int in1R, 
    unsigned int in2R, 
    unsigned int enL, 
    unsigned int in1L, 
    unsigned int in2L):
    motorRight(enR, in1R, in2R),
    motorLeft(enL, in1L, in2L) {

}


//Initializes the two motors
//Must be called on startup!
void MotorsController::init() {
    motorRight.init();
    motorLeft.init();
}


//Returns the max motor speed
unsigned int MotorsController::getMaxSpeed() {
    return kMaxMotorSpeed;
}


//Updates the speed of all the motors
void MotorsController::updateSpeed(unsigned int speedR, unsigned int speedL) {
    motorRight.setSpeed(speedR);
    motorLeft.setSpeed(speedL);
}


//Updates the direction of all the motors
void MotorsController::updateDirection(HBridgeMotor::Direction directionR, 
                                        HBridgeMotor::Direction directionL) {
    motorRight.setDirection(directionR);
    motorLeft.setDirection(directionL);
}
