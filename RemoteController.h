#ifndef __REMOTE_CONTROLLER_H
#define __REMOTE_CONTROLLER_H

#include <stdint.h>
#include <Bluepad32.h>


struct RemoteControllerData {
  const bool error;
  const int16_t axisX;
  const int16_t axisY;
};


class RemoteController {
public:
  RemoteController();
  bool run();
  RemoteControllerData getData();

private:
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