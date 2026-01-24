#ifndef __REMOTE_CONTROLLER_H
#define __REMOTE_CONTROLLER_H

#include <string>
#include <functional>
#include <Bluepad32.h>
#include <esp_bt.h>


struct RemoteControllerData {
  const bool error;
  const int axisX;
  const int axisY;
  const unsigned int rightTrigger;
  const unsigned int leftTrigger;
  const bool rightButton;
  const bool leftButton;
};


class RemoteController {
  typedef std::function<void()> ControllerDisconnectCallback;

public:
  //Methods//
  RemoteController();
  ~RemoteController();
  void init();
  bool run();
  RemoteControllerData getData() const;
  void registerDisconnectCallback(const ControllerDisconnectCallback& callback);

private:
  //Constants//
  static const std::string kTag;
  static constexpr unsigned int kRightButtonMask    {0x20};
  static constexpr unsigned int kLeftButtonMask     {0x10};

  //Variables//
  ControllerPtr controller;
  ControllerDisconnectCallback disconnectCallback;

  //Methods//
  bool isConnected(ControllerPtr ctl) const;
  void onConnectedController(ControllerPtr ctl);
  void onDisconnectedController(ControllerPtr ctl);
  void dumpGamepad(ControllerPtr ctl) const;
  void changeTxPower(esp_power_level_t pwrLvl);
  int powerLvlToDbm(esp_power_level_t pwrLvl) const;
};

#endif // __REMOTE_CONTROLLER_H