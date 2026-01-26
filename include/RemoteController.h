#pragma once

#include <functional>
#include <Bluepad32.h>
#include <esp_bt.h>


struct RemoteControllerData {
  bool error;
  int axisX;
  int axisY;
  unsigned int rightTrigger;
  unsigned int leftTrigger;
  bool rightButton;
  bool leftButton;
};


class RemoteController {
  typedef std::function<void()> ControllerDisconnectCallback;

public:
  //Methods//
  RemoteController();
  ~RemoteController();
  void init();
  bool run();
  const RemoteControllerData getData() const;
  void registerDisconnectCallback(const ControllerDisconnectCallback& callback);

private:
  //Constants//
  static constexpr const char* kTag {"Remote Controller"};
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