#include "../include/MotorsController.h"


// Static Constants Initialization //
const std::string MotorsController::kTag            {"Motors Controller"};


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


/**
 * @brief Initializes the motor pins for both the right and left motors.
 * @note This function must be called, typically in the setup() function,
 * before any other motor operations are performed.
 */
void MotorsController::init() {
    motorRight.init();
    motorLeft.init();
}


/**
 * @brief Updates the speed for both the right and left motors.
 */
void MotorsController::updateSpeed(unsigned int speedR, unsigned int speedL) {
    motorRight.setSpeed(speedR);
    motorLeft.setSpeed(speedL);
}


/**
 * @brief Updates the direction for both the right and left motors.
 */
void MotorsController::updateDirection(HBridgeMotor::Direction directionR, 
                                        HBridgeMotor::Direction directionL) {
    motorRight.setDirection(directionR);
    motorLeft.setDirection(directionL);
}
