#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "../include/DriveLogic.h"
#include "../include/HBridgeMotor.h"


// Static Constants Initialization //
constexpr const char* DriveLogic::kTag;
constexpr DriveLogicConfig DriveLogic::kDefaultConfig;

DriveLogic::DriveLogic(const DriveLogicConfig& driveConfig) :
    kTriggerMaxVal(driveConfig.triggerMaxVal),
    kJoystickMaxVal(driveConfig.joystickMaxVal),
    kTriggerDeadzone(driveConfig.triggerDeadzone),
    kJoystickDeadzone(driveConfig.joystickDeadzone),
    kMaxMotorSpeed(driveConfig.maxMotorSpeed) {
  
}


/**
 * @brief Converts the provided controller triggers values to the speed of the outer motor.
 *
 * If both the triggers values are within the deadzone, 0 is returned. 
 * Otherwise, the returned value is the outer wheel speed from '-kMaxMotorSpeed' to 'kMaxMotorSpeed'
 * where: negative -> backward, positive -> forward, 0 -> stop.
 * 
 * Right trigger -> Drive forward
 * Left trigger  -> Drive backward
 */
int DriveLogic::triggersToOuterWheelSpeed(unsigned int triggerR, unsigned int triggerL) const {
    if(triggerR < kTriggerDeadzone && triggerL < kTriggerDeadzone) {
        ESP_LOGV(kTag, "Not Driving! Both the right & left triggers are within the deadzone.");
        return 0;
    }

    float speed {};
    if(triggerR >= triggerL) {
        speed = (static_cast<float>(triggerR) / kTriggerMaxVal) * kMaxMotorSpeed;
    }
    else {
        speed = -(static_cast<float>(triggerL) / kTriggerMaxVal) * kMaxMotorSpeed;
    }

    return static_cast<int>(speed);
}


/**
 * @brief Calculates the speed of the inner wheel for turning.
 *
 * Reduces the provided outerWheelSpeed based on the magnitude of the steering input('joystickX').
 * This implements differential steering to facilitate the turn.
 *
 * @note If 'joystickX' is within the deadzone('kJoystickDeadzone'), the inner wheel
 *           speed will be equal the provided outer wheel speed (straight driving).
 */
int DriveLogic::calculateInnerWheelSpeed(int joystickX, int outerWheelSpeed) const {
    int steerMagnitude = std::abs(joystickX);
    if(steerMagnitude < kJoystickDeadzone) {
        ESP_LOGV(kTag, "Not steering! Steer magnitude is within the joystick deadzone.");
        return outerWheelSpeed;
    }

    float ratio = 1 - static_cast<float>(steerMagnitude) / kJoystickMaxVal;
    return static_cast<int>(outerWheelSpeed * ratio);
}


/**
 * @brief Populates the final motor control output structure ('out').
 *
 * Maps the calculated inner and outer wheel speeds to the appropriate Left and Right
 * motor control parameters (direction and absolute speed) based on the steering input ('joystickX').
 *
 * 'joystickX' >= 0 -> Turn right
 * 'joystickX' < 0  -> Turn left
 */
void DriveLogic::populateDriveLogicOut(
    int joystickX, 
    int outerWheelSpeed, 
    int innerWheelSpeed, 
    DriveLogicOut& out) const {

    if(joystickX >= 0) {
        out.directionR = speedToDirection(innerWheelSpeed);
        out.directionL = speedToDirection(outerWheelSpeed);

        out.speedR = std::abs(innerWheelSpeed);
        out.speedL = std::abs(outerWheelSpeed);
    }
    else {
        out.directionR = speedToDirection(outerWheelSpeed);
        out.directionL = speedToDirection(innerWheelSpeed);

        out.speedR = std::abs(outerWheelSpeed);
        out.speedL = std::abs(innerWheelSpeed);
    }
}


/**
 * @brief Converts a signed speed value into a motor direction enum(Forward/Backward/Stop).
 */
 HBridgeMotor::Direction DriveLogic::speedToDirection(int speed) const {
    if(speed > 0)         return HBridgeMotor::Direction::kForward;
    else if(speed < 0)    return HBridgeMotor::Direction::kBackward;
    return HBridgeMotor::Direction::kStop;
}


/**
 * @brief Processes raw remote controller inputs to determine final motor outputs.
 *
 * This function orchestrates the driving logic flow by:
 * 1. Determining the base/outer wheel speed from the triggers ('triggerR'/'triggerL').
 * 2. Calculating the necessary inner wheel reduction based on the joystick steering ('joystickX').
 * 3. Mapping these calculated speeds to the final Left & Right motor speed and direction values in 'out'.
 */
void DriveLogic::handleRemoteControllerInput(
    int joystickX,
    unsigned int triggerR,
    unsigned int triggerL,
    DriveLogicOut& out) const {

    int outerWheelSpeed = triggersToOuterWheelSpeed(triggerR, triggerL);
    int innerWheelSpeed = calculateInnerWheelSpeed(joystickX, outerWheelSpeed);
    populateDriveLogicOut(joystickX, outerWheelSpeed, innerWheelSpeed, out);

    ESP_LOGV(kTag, "Joystick X: %d | SpeedR: %u | SpeedL: %u | DirectionR: %d | DirectionL: %d", 
                                        joystickX, out.speedR, out.speedL, out.directionR, out.directionL);
}