#ifndef __ROBOT_H
#define __ROBOT_H

#include <string>
#include "MotorsController.h"
#include "RemoteController.h"
#include "DriveLogic.h"
#include "BladeController.h"
#include "Simulator.h"


class Robot {
public:
  //Methods//
  Robot();
  void init();
  void run();

private:
  //Constants//
  static const std::string kTag;

  //Variables//
  RemoteController remoteController;
  MotorsController motorsController;
  DriveLogic driveLogic;
  BladeController bladeController;
  Simulator simulator;

  //Methods//
  void updateMotors(RemoteControllerData data);
  void updateBladeMotor(RemoteControllerData data);
  void updateWheelMotors(RemoteControllerData data);
  void onRemoteControllerDisconnect();
};

#endif // __ROBOT_H