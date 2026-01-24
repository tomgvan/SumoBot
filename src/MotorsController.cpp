#include <Arduino.h>
#include "../include/MotorsController.h"


// Static Constants Initialization //
constexpr const char* MotorsController::kTag;
constexpr MotorsControllerConfig MotorsController::kDefaultConfig;


MotorsController::MotorsController(const MotorsControllerConfig& motorsConfig):
    motorRight(
        motorsConfig.rightPins, 
        motorsConfig.minMotorSpeed, 
        motorsConfig.maxMotorSpeed
    ),
    motorLeft(
        motorsConfig.leftPins, 
        motorsConfig.minMotorSpeed, 
        motorsConfig.maxMotorSpeed
    ) {

}


/**
 * @brief Initializes the motor pins for both the right and left motors.
 * @note This function must be called, typically in the setup() function,
 * before any other motor operations are performed.
 */
void MotorsController::init() {
    ESP_LOGI(kTag, "Initializing MotorsController.\n");

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


/**
 * @brief Stops the driving motors.
 */
void MotorsController::stop() {
    updateDirection(HBridgeMotor::Direction::kStop, HBridgeMotor::Direction::kStop);
    updateSpeed(motorRight.getMinSpeed(), motorLeft.getMinSpeed());
}