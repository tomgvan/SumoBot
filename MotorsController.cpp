#include "MotorsController.h"


// Static Constants Initialization
const std::string MotorsController::kTag = "Motors Controller";
const uint32_t MotorsController::kDelayMs = 5;
const uint8_t MotorsController::kMaxMotorSpeed = 255;
const uint8_t MotorsController::kMinMotorSpeed = 0;


MotorsController::MotorsController(
    uint8_t enR, 
    uint8_t in1R, 
    uint8_t in2R, 
    uint8_t enL, 
    uint8_t in1L, 
    uint8_t in2L):
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
uint8_t MotorsController::getMaxSpeed() {
    return kMaxMotorSpeed;
}


//Updates the speed of all the motors
void MotorsController::updateSpeed(uint8_t speedR, uint8_t speedL) {
    motorRight.setSpeed(speedR);
    motorLeft.setSpeed(speedL);
}


//Updates the direction of all the motors
void MotorsController::updateDirection(HBridgeMotor::Direction directionR, 
                                        HBridgeMotor::Direction directionL) {
    motorRight.setDirection(directionR);
    motorLeft.setDirection(directionL);
}
