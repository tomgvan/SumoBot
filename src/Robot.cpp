#include "../include/Robot.h"


// Static Constants Initialization //
constexpr const char* Robot::kTag;


Robot::Robot():
  remoteController(),
  motorsController(),
  driveLogic(),
  bladeController() {

}


/**
 * @brief Initializes all of the robot's hardware controllers.
 */
void Robot::init() {
  ESP_LOGI(kTag, "Initializing Robot.\n");

  motorsController.init();
  bladeController.init();
  remoteController.init();

  remoteController.registerDisconnectCallback(
    [this]() {
      this->onRemoteControllerDisconnect();
    }
  );
}


/**
 * @brief Runs the robot's main update logic.
 * @note Must be called continuously from the main loop.
 *
 * Polls the remote controller, If valid update avilable 
 * pass it to the motors controller that will update their position accordingly.
 */
void Robot::run() {
  const bool updated {remoteController.run()};

  if(updated) {
      const RemoteControllerData data {remoteController.getData()};

    if(!data.error) {
      updateMotors(data);
    }
  }

  bladeController.run();
}


/**
 * @brief Updates the wheel motors based on joystick input.
 *
 * Takes the controller data, uses DriveLogic to calculate speeds and
 * directions, and sends the commands to the MotorsController.
 */
void Robot::updateWheelMotors(RemoteControllerData data) {
  DriveLogicOut driveLogicOut = driveLogic.handleRemoteControllerInput(
    data.axisX, 
    data.rightTrigger,
    data.leftTrigger
  );

  motorsController.updateDirection(driveLogicOut.directionR, driveLogicOut.directionL);
  motorsController.updateSpeed(driveLogicOut.speedR, driveLogicOut.speedL);
}


/**
 * @brief Add a lift/lower request for the blade servo motor controller.
 *
 * Uses the right button to lift and the left button to lower the blade.
 */
void Robot::updateBladeMotor(RemoteControllerData data) {
  if(data.rightButton)      bladeController.lift();
  else if(data.leftButton)  bladeController.lower();
}


/**
 * @brief Updates all motors(wheels and blade) from remtoe controller data.
 */
void Robot::updateMotors(RemoteControllerData data) {
  updateWheelMotors(data);
  updateBladeMotor(data);
}


/**
 * @brief Stops the robot movement when remote controller disconnects.
 *
 * Prevents the robot from going out of the arena if the remote controller disconnects.
 */
void Robot::onRemoteControllerDisconnect() {
  ESP_LOGI(kTag, "Remote controller disconnected callback");
  motorsController.stop();
}