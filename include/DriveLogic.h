#ifndef __DRIVE_LOGIC_H
#define __DRIVE_LOGIC_H

#include <string>
#include "HBridgeMotor.h"


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
  ) const;

private:
  //Constants//
  static const std::string kTag;
  const double kJoystickMaxVal;
  const unsigned int kJoystickDeadzone;
  const unsigned int kMaxMotorSpeed;

  //Methods//
  bool joystickToSpeed(int x, int y, int& speedR, int& speedL) const;
  HBridgeMotor::Direction speedToDirection(int speed) const;
  bool isInsideDeadzone(int x, int y, int& speedR, int& speedL) const;
  void calcDifferentialSpeed(int x, int y, double& unscaledR, double& unscaledL) const;
  double calcNormalizationFactor(double unscaledR, double unscaledL) const;
  void setPointTurnUnscaled(double steer, double& unscaledR, double& unscaledL) const;
  void setStraightUnscaled(double throttle, double& unscaledR, double& unscaledL) const;
  void setArcTurnUnscaled(double x, 
                          double y, 
                          double throttle, 
                          double steer,  
                          double& unscaledR, 
                          double& unscaledL) const;
};

#endif // __DRIVE_LOGIC_H