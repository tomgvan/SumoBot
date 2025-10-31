#include "Gpios.h"
#include "MotorsController.h"
#include "RemoteController.h"
#include "DriveLogic.h"


RemoteController remoteController;

MotorsController motorsController(
  gpios::kMotorRightEn, 
  gpios::kMotorRightIn1, 
  gpios::kMotorRightIn2, 
  gpios::kMotorLeftEn, 
  gpios::kMotorLeftIn1, 
  gpios::kMotorLeftIn2
);

DriveLogic driveLogic(
  remoteController.getJoystickMax(),
  remoteController.getJoystickDeadZone(),
  motorsController.getMaxSpeed()
);


void setup() {
  Serial.begin(115200);

  motorsController.init();
  remoteController.init();
}


void loop() {
  bool updated {remoteController.run()};

  if(updated) {
    RemoteControllerData data {remoteController.getData()};
    if(!data.error)
      updateMotors(data);
  }

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    // delay(150);
}


void updateMotors(RemoteControllerData data) {
  uint8_t speedR{};
  uint8_t speedL{};
  HBridgeMotor::Direction directionR{};
  HBridgeMotor::Direction directionL{};

  bool res = driveLogic.handleJoystickInput(
    data.axisX, 
    data.axisY, 
    speedR, 
    speedL, 
    directionR, 
    directionL
  );

  if(res) {
    motorsController.updateDirection(directionR, directionL);
    motorsController.updateSpeed(speedR, speedL);
  }
}