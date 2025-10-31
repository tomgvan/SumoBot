#ifndef __REMOTE_CONTROLLER_H
#define __REMOTE_CONTROLLER_H

#include <stdint.h>
#include <string>
#include <Bluepad32.h>


struct RemoteControllerData {
  const bool error;
  const int16_t axisX;
  const int16_t axisY;
};


class RemoteController {
public:
  RemoteController();
  ~RemoteController();
  void init();
  bool run();
  RemoteControllerData getData();
  double getJoystickMax();
  int16_t getJoystickDeadZone() const;
  

private:
  //Constants//
  static const std::string kTag;
  static const int16_t kJoystickDeadzone;
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