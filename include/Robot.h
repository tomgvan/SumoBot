#pragma once

#include "MotorsController.h"
#include "RemoteController.h"
#include "DriveLogic.h"
#include "BladeController.h"


class Robot {
public:
  //Methods//
  Robot();
  void init();
  void run();

private:
  //Constants//
  static constexpr const char* kTag {"Robot"};

  //Variables//
  RemoteController remoteController;
  MotorsController motorsController;
  DriveLogic driveLogic;
  BladeController bladeController;

  //Methods//
  void updateMotors(RemoteControllerData data);
  void updateBladeMotor(RemoteControllerData data);
  void updateWheelMotors(RemoteControllerData data);
  void onRemoteControllerDisconnect();
};