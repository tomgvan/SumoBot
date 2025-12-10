#include "../include/Robot.h"
#include "../include/Gpios.h"


// Static Constants Initialization //
const std::string Robot::kTag  {"Robot"};


Robot::Robot():
  remoteController(),
  motorsController(
      gpios::kMotorRightEn, 
      gpios::kMotorRightIn1, 
      gpios::kMotorRightIn2, 
      gpios::kMotorLeftEn, 
      gpios::kMotorLeftIn1, 
      gpios::kMotorLeftIn2
  ),
  driveLogic(
      RemoteController::kMaxTriggerVal,
      RemoteController::kMaxJoystickVal,
      RemoteController::kTriggerDeadzone,
      RemoteController::kJoystickDeadzone,
      HBridgeMotor::kMaxMotorSpeed
  ),
  bladeController(
      gpios::kBladeMotorPin,
      750,
      2250,
      50,
      60/0.2
  ) {

}


/**
 * @brief Initializes all of the robot's hardware controllers.
 */
void Robot::init() {
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
      RemoteControllerData data {remoteController.getData()};

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
  DriveLogicOut driveLogicOut {
    0,
    0,
    HBridgeMotor::Direction::kStop,
    HBridgeMotor::Direction::kStop
  };

  driveLogic.handleRemoteControllerInput(
    data.axisX, 
    data.rightTrigger,
    data.leftTrigger,
    driveLogicOut
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
 * @brief On remote controller disconnect stopping the wheel motors.
 */
void Robot::onRemoteControllerDisconnect() {
  ESP_LOGI(kTag.c_str(), "Remote controller disconnected callback");

  motorsController.updateDirection(
    HBridgeMotor::Direction::kStop, 
    HBridgeMotor::Direction::kStop
  );

  motorsController.updateSpeed(
    HBridgeMotor::kMinMotorSpeed,
    HBridgeMotor::kMinMotorSpeed
  );
}