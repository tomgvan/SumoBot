#pragma once

#include "HBridgeMotor.h"


struct DriveLogicOut {
  unsigned int speedR;
  unsigned int speedL;
  HBridgeMotor::Direction directionR;
  HBridgeMotor::Direction directionL;
};

struct DriveLogicConfig {
  unsigned int triggerMaxVal;
  unsigned int joystickMaxVal;
  unsigned int triggerDeadzone;
  unsigned int joystickDeadzone;
  unsigned int maxMotorSpeed;
};


class DriveLogic {
public:
  //Constants//
  static constexpr DriveLogicConfig kDefaultConfig {
    .triggerMaxVal    = 1023,
    .joystickMaxVal   = 512,
    .triggerDeadzone  = 100,
    .joystickDeadzone = 50,
    .maxMotorSpeed    = 255
  };

  //Methods//
  explicit DriveLogic(const DriveLogicConfig& driveConfig = kDefaultConfig);
  void handleRemoteControllerInput(
    int x, 
    unsigned int triggerR,
    unsigned int triggerL,
    DriveLogicOut& out
  ) const;



private:
  //Constants//
  static constexpr const char* kTag {"Drive Logic"};
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