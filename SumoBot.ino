#include "include/Robot.h"


Robot robot;


void setup() {
  Serial.begin(115200);
  while(!Serial);

  ESP_LOGV("Main", "Initializing robot...");

  robot.init();
  ESP_LOGV("Main", "Robot Initialized");
}


void loop() {
  robot.run();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  vTaskDelay(1);
}