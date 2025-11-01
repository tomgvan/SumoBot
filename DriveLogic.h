#ifndef __DRIVE_LOGIC_H
#define __DRIVE_LOGIC_H

#include "HBridgeMotor.h"
#include <string>


class DriveLogic {
public:
  DriveLogic(double joystickMaxVal, unsigned int joystickDeadzone, unsigned int maxMotorSpeed);
  bool handleJoystickInput(
    int x, 
    int y, 
    unsigned int& speedR, 
    unsigned int& speedL, 
    HBridgeMotor::Direction& directionR, 
    HBridgeMotor::Direction& directionL
  );


private:
  //Constants//
  static const std::string kTag;
  const double kJoystickMaxVal;
  const unsigned int kJoystickDeadzone;
  const unsigned int kMaxMotorSpeed;

  //Methods//
  bool joystickToSpeed(int x, int y, int& speedR, int& speedL);
  HBridgeMotor::Direction speedToDirection(int speed);
};
#endif