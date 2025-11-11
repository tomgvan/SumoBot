#include "DriveLogic.h"
#include "HBridgeMotor.h"
#include <Arduino.h>
#include <algorithm>
#include <cmath>


// Static Constants Initialization //
const std::string DriveLogic::kTag {"Drive Logic"};


DriveLogic::DriveLogic(
    double joystickMaxVal, 
    unsigned int joystickDeadzone, 
    unsigned int maxMotorSpeed) :
        kJoystickMaxVal(joystickMaxVal),
        kJoystickDeadzone(joystickDeadzone),
        kMaxMotorSpeed(maxMotorSpeed) {
  
}


/**
 * @brief Translates joystick (x, y) into final motor speeds and directions.
 *
 * Calls `joystickToSpeed` to get signed speeds, then converts them
 * into unsigned speeds (0 to kMaxMotorSpeed) and `HBridgeMotor::Direction`
 * enums for output.
 */
bool DriveLogic::handleJoystickInput(
                          int x, 
                          int y, 
                          unsigned int& speedR, 
                          unsigned int& speedL, 
                          HBridgeMotor::Direction& directionR, 
                          HBridgeMotor::Direction& directionL
                        ) const {
  int signedSpeedR {};
  int signedSpeedL {};

  bool res {joystickToSpeed(x, y, signedSpeedR, signedSpeedL)};

  directionR = speedToDirection(signedSpeedR);
  directionL = speedToDirection(signedSpeedL);

  speedR = abs(signedSpeedR);
  speedL = abs(signedSpeedL);
  
  Serial.printf("%s - SpeedR: %u | SpeedL: %u | DirectionR: %d | DirectionL: %d\n", 
                                         kTag.c_str(), speedR, speedL, directionR, directionL);

  return res;
}


/**
 * @brief Converts a signed speed value into a motor direction enum.
 */
 HBridgeMotor::Direction DriveLogic::speedToDirection(int speed) const {
    if(speed > 0)         return HBridgeMotor::Direction::kForward;
    else if(speed < 0)    return HBridgeMotor::Direction::kBackward;
    return HBridgeMotor::Direction::kStop;
}


/**
 * @brief Checks if joystick (x, y) is in the deadzone.
 * 
 * Returns true if inside the deadzone and false otherwise.
 */
bool DriveLogic::isInsideDeadzone(int x, int y, int& speedR, int& speedL) const {
    if (hypot(static_cast<double>(x), static_cast<double>(y)) < kJoystickDeadzone) {
        Serial.printf("%s - Joystick input magnitude inside deadzone. Returning motor speed 0.\n", kTag.c_str());
        speedR = 0;
        speedL = 0;
        return true;
    }

    return false;
}


/**
 * @brief Calculates differential steering motor speeds.
 *
 * Normalizes joystick Y (inverted) to throttle and X to steer [-1.0, 1.0].
 * It then mixes them: right = (throttle - steer), left = (throttle + steer).
 */
void DriveLogic::calcDifferentialSpeed(int x, int y, double& unscaledR, double& unscaledL) const {
    //Sperate components and normalize(-1 to +1)
    double throttle {-static_cast<double>(y) / kJoystickMaxVal};
    double steer {static_cast<double>(x) / kJoystickMaxVal};
    Serial.printf("%s - Normalized. Throttle: %f | Steer: %f\n", kTag.c_str(), throttle, steer);

    //Add the components toghter(-2 to +2)
    unscaledR = throttle - steer;
    unscaledL = throttle + steer;
    Serial.printf("%s - Unscaled R: %f | Unscaled L: %f\n", kTag.c_str(), unscaledR, unscaledL);
}


/**
 * @brief Calculates a factor (<= 1.0) to cap the max motor speed
 * at 1.0 while preserving the steering ratio.
 */
double DriveLogic::calcNormalizationFactor(double unscaledR, double unscaledL) const {
    double maxMagnitude {max(abs(unscaledR), abs(unscaledL))};
    double factor {1.0};
    if (maxMagnitude > 1.0)
        factor = 1.0 / maxMagnitude;
    // log(VERBOSE, TAG, "Speed scale: " + scale);

    return factor;
}


/**
 * @brief Converts joystick (x, y) coordinates to final motor speeds.
 *
 * Applies the deadzone, calculates the differential steering mix, and proportionally scales
 * the output to the [-kMaxMotorSpeed, +kMaxMotorSpeed] range.
 */
bool DriveLogic::joystickToSpeed(int x, int y, int& speedR, int& speedL) const {
    if (isInsideDeadzone(x, y, speedR, speedL))
        return false;

    double unscaledRight {};
    double unscaledLeft {};
    calcDifferentialSpeed(x, y, unscaledRight, unscaledLeft);

    double factor {calcNormalizationFactor(unscaledRight, unscaledLeft)};

    //Finalize motors speed(0 to kMaxMotorSpeed)
    speedR = (unscaledRight * factor) * kMaxMotorSpeed;
    speedL = (unscaledLeft * factor) * kMaxMotorSpeed;
    // log(INFO, TAG, "R Speed: " + motorRight + " L Speed: " + motorLeft);

    return true;
}