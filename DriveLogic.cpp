#include "DriveLogic.h"
#include "HBridgeMotor.h"
#include <Arduino.h>
#include <algorithm>
#include <cmath>


// Static Constants Initialization
const std::string DriveLogic::kTag = "Drive Logic";


DriveLogic::DriveLogic(
                        double joystickMaxVal, 
                        int16_t joystickDeadzone, 
                        uint8_t maxMotorSpeed) :
                                  kJoystickMaxVal(joystickMaxVal),
                                  kJoystickDeadzone(joystickDeadzone),
                                  kMaxMotorSpeed(maxMotorSpeed) {
  
}


//Translate the provided joystick x & y values to speed and direction for two motored wheels
//Assigns the calculated speed and direction to the params that are passed by reference
//Speed - 0 to kMaxMotorSpeed
//Direction Stop/Forward/Backward
bool DriveLogic::handleJoystickInput(
                          int16_t x, 
                          int16_t y, 
                          uint8_t& speedR, 
                          uint8_t& speedL, 
                          HBridgeMotor::Direction& directionR, 
                          HBridgeMotor::Direction& directionL
                        ) {
  int16_t signedSpeedR {};
  int16_t signedSpeedL {};

  bool res {joystickToSpeed(x, y, signedSpeedR, signedSpeedL)};

  directionR = speedToDirection(signedSpeedR);
  directionL = speedToDirection(signedSpeedL);

  speedR = abs(signedSpeedR);
  speedL = abs(signedSpeedL);
  
  Serial.printf("%s - SpeedR: %u | SpeedL: %u | DirectionR: %d | DirectionL: %d\n", 
                                         kTag, speedR, speedL, directionR, directionL);

  return res;
}


//Converts the provided motor speed(-100 to +100) to direction(forward/backward/stop)
HBridgeMotor::Direction DriveLogic::speedToDirection(int16_t speed) {
    if(speed > 0)         return HBridgeMotor::Direction::kForward;
    else if(speed < 0)    return HBridgeMotor::Direction::kBackward;
    return HBridgeMotor::Direction::kStop;
}


//Calculates from the provided x & y values of a joystick the speed(-kMaxMotorSpeed to +kMaxMotorSpeed) for the two motors
//assigns the final value to the provided parameters which are passed by reference
bool DriveLogic::joystickToSpeed(int16_t x, int16_t y, int16_t& speedR, int16_t& speedL) {
    //
    if (hypot(static_cast<double>(x), static_cast<double>(y)) < kJoystickDeadzone) {
        Serial.printf("%s - Joystick input magnitude inside deadzone. Returning motor speed 0.\n", kTag);
        speedR = 0;
        speedL = 0;
        return false;
    }
    
    //Sperate components and normalize(-1 to +1)
    double throttle {-static_cast<double>(y) / kJoystickMaxVal};
    double steer {static_cast<double>(x) / kJoystickMaxVal};
    Serial.printf("%s - Normalized. Throttle: %d | Steer: %d\n", kTag, throttle, steer);

    //Add the components toghter(-2 to +2)
    double rawRight {throttle - steer};
    double rawLeft {throttle + steer};
    Serial.printf("%s - Raw R: %f | Raw L: %f\n", kTag, rawRight, rawLeft);

    //
    double maxMagnitude {max(abs(rawLeft), abs(rawRight))};
    double scale {1.0};
    if (maxMagnitude > 1.0)
        scale = 1.0 / maxMagnitude;
    // log(VERBOSE, TAG, "Speed scale: " + scale);

    //Finalize motors speed(0 to kMaxMotorSpeed)
    speedR = (rawRight * scale) * kMaxMotorSpeed;
    speedL = (rawLeft * scale) * kMaxMotorSpeed;
    // log(INFO, TAG, "R Speed: " + motorRight + " L Speed: " + motorLeft);

    return true;
}