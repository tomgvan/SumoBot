#ifndef __DRIVE_LOGIC_H
#define __DRIVE_LOGIC_H

#include <stdint.h>
#include <string>
#include "HBridgeMotor.h"


class DriveLogic {
public:
  DriveLogic(double joystickMaxVal, int16_t joystickDeadzone, uint8_t maxMotorSpeed);
  bool handleJoystickInput(
    int16_t x, 
    int16_t y, 
    uint8_t& speedR, 
    uint8_t& speedL, 
    HBridgeMotor::Direction& directionR, 
    HBridgeMotor::Direction& directionL
  );


private:
  //Constants//
  static const std::string kTag;
  const double kJoystickMaxVal;
  const int16_t kJoystickDeadzone;
  const uint8_t kMaxMotorSpeed;

  //Methods//
  bool joystickToSpeed(int16_t x, int16_t y, int16_t& speedR, int16_t& speedL);
  HBridgeMotor::Direction speedToDirection(int16_t speed);
};
#endif