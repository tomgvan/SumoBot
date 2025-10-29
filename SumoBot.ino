#include "Gpios.h"
#include "MotorsController.h"
#include "RemoteController.h"


MotorsController motorsController(
  GPIO_MOTOR_R_EN, 
  GPIO_MOTOR_R_IN1, 
  GPIO_MOTOR_R_IN2, 
  GPIO_MOTOR_L_EN, 
  GPIO_MOTOR_L_IN1, 
  GPIO_MOTOR_L_IN2
);

RemoteController remoteController;


void setup() {
  Serial.begin(115200);
}


void loop() {
  bool updated = remoteController.run();

  if(updated) {
    RemoteControllerData data = remoteController.getData();
    if(!data.error)
      motorsController.updateMotorsFromInput(data.axisX, data.axisY);
  }

  motorsController.run();



    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    // delay(150);
}
