#ifndef __REMOTE_CONTROLLER_H
#define __REMOTE_CONTROLLER_H

#include <string>
#include <Bluepad32.h>


struct RemoteControllerData {
  const bool error;
  const int axisX;
  const int axisY;
};


class RemoteController {
public:
  RemoteController();
  ~RemoteController();
  void init();
  bool run();
  RemoteControllerData getData();
  double getJoystickMax();
  int getJoystickDeadZone() const;
  

private:
  //Constants//
  static const std::string kTag;
  static unsigned const int kJoystickDeadzone;
  static const double kMaxJoystickVal;

  //Variables//
  ControllerPtr controller;

  //Methods//
  bool isConnected(ControllerPtr ctl);
  void onConnectedController(ControllerPtr ctl);
  void onDisconnectedController(ControllerPtr ctl);
  void dumpGamepad(ControllerPtr ctl);
  bool processGamepad();
};


#endif // __REMOTE_CONTROLLER_H