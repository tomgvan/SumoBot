#ifndef __REMOTE_CONTROLLER_H
#define __REMOTE_CONTROLLER_H

#include <string>
#include <Bluepad32.h>


struct RemoteControllerData {
  const bool error;
  const int axisX;
  const int axisY;
  const unsigned int rightTrigger;
  const unsigned int leftTrigger;
};


class RemoteController {
public:
  RemoteController();
  ~RemoteController();
  void init();
  bool run();
  RemoteControllerData getData() const;
  double getJoystickMax() const;
  int getJoystickDeadZone() const;
  unsigned int getTriggerMax() const;
  unsigned int getTriggerMin() const;
  
private:
  //Constants//
  static const std::string kTag;
  static const unsigned int kJoystickDeadzone;
  static const double kMaxJoystickVal;
  static const unsigned int kMinTriggerVal;
  static const unsigned int kMaxTriggerVal;

  //Variables//
  ControllerPtr controller;

  //Methods//
  bool isConnected(ControllerPtr ctl) const;
  void onConnectedController(ControllerPtr ctl);
  void onDisconnectedController(ControllerPtr ctl);
  void dumpGamepad(ControllerPtr ctl) const;
  bool processGamepad();
};

#endif // __REMOTE_CONTROLLER_H