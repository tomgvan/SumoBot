#ifndef __DRIVE_LOGIC_H
#define __DRIVE_LOGIC_H

#include <string>
#include "HBridgeMotor.h"


struct DriveLogicOut {
    unsigned int speedR;
    unsigned int speedL;
    HBridgeMotor::Direction directionR;
    HBridgeMotor::Direction directionL;
};


class DriveLogic {
public:
  //Methods//
  DriveLogic(
    unsigned int triggerMaxVal,
    unsigned int joystickMaxVal, 
    unsigned int triggerDeadzone,
    unsigned int joystickDeadzone, 
    unsigned int maxMotorSpeed
    );
  void handleRemoteControllerInput(
    int x, 
    unsigned int triggerR,
    unsigned int triggerL,
    DriveLogicOut& out
  ) const;

private:
  //Constants//
  static const std::string kTag;
  const float kTriggerMaxVal;
  const float kJoystickMaxVal;
  const unsigned int kTriggerDeadzone;
  const unsigned int kJoystickDeadzone;
  const unsigned int kMaxMotorSpeed;

  //Methods//
  HBridgeMotor::Direction speedToDirection(int speed) const;
  int triggersToOuterWheelSpeed(unsigned int triggerR, unsigned int triggerL) const;
  int calculateInnerWheelSpeed(int joystickX, int outerWheelSpeed) const;
  void populateDriveLogicOut(int joystickX, int outerWheelSpeed, int innerWheelSpeed, DriveLogicOut& out) const;
};

#endif // __DRIVE_LOGIC_H