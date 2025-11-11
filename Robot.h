#ifndef __ROBOT_H
#define __ROBOT_H

#include "MotorsController.h"
#include "RemoteController.h"
#include "DriveLogic.h"
#include "BladeController.h"


class Robot {
public:
  Robot();
  void init();
  void run();

private:
  //Variables//
  RemoteController remoteController;
  MotorsController motorsController;
  DriveLogic driveLogic;
  BladeController bladeController;

  //Methods//
  void updateMotors(RemoteControllerData data);
  void updateBladeMotor(RemoteControllerData data);
  void updateWheelMotors(RemoteControllerData data);
};

#endif // __ROBOT_H