#include "Robot.h"
#include "Gpios.h"


#define SIMULATOR


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
      remoteController.getJoystickMax(),
      remoteController.getJoystickDeadZone(),
      motorsController.getMaxSpeed()
  ),
  bladeController(
      gpios::kBladeMotorPin,
      750,
      2250,
      50,
      60/0.2
  ),
  simulator(2000) {

}


/**
 * @brief Initializes all of the robot's hardware controllers.
 */
void Robot::init() {
  motorsController.init();
  bladeController.init();
  remoteController.init();
}


/**
 * @brief Runs the robot's main update logic.
 * @note Must be called continuously from the main loop.
 *
 * Polls the remote controller, If valid update avilable 
 * pass it to the motors controller that will update their position accordingly.
 */
void Robot::run() {
  
#ifdef SIMULATOR
    RemoteControllerData data {simulator.getData()};
    if(!data.error) {
      updateMotors(data);
    }
#else
  const bool updated {remoteController.run()};

  if(updated) {
      RemoteControllerData data {remoteController.getData()};

    if(!data.error) {
      updateMotors(data);
    }
  }
#endif

  bladeController.run();
}


/**
 * @brief Updates the wheel motors based on joystick input.
 *
 * Takes the controller data, uses DriveLogic to calculate speeds and
 * directions, and sends the commands to the MotorsController.
 */
void Robot::updateWheelMotors(RemoteControllerData data) {
  unsigned int speedR{};
  unsigned int speedL{};
  HBridgeMotor::Direction directionR{};
  HBridgeMotor::Direction directionL{};

  const bool inputHandled = driveLogic.handleJoystickInput(
    data.axisX, 
    data.axisY, 
    speedR, 
    speedL, 
    directionR, 
    directionL
  );

  if(inputHandled) {
    motorsController.updateDirection(directionR, directionL);
    motorsController.updateSpeed(speedR, speedL);
  }
}


/**
 * @brief Add a lift/lower request for the blade servo motor controller.
 *
 * Uses the right trigger to lift and the left trigger to lower the blade.
 * The trigger with the higher value dictate if a lift(right trigger) or lower(left trigger) is called.
 */
void Robot::updateBladeMotor(RemoteControllerData data) {
  if(data.rightTrigger == 0 && data.leftTrigger == 0)
    return;

  if(data.rightTrigger > data.leftTrigger)
    bladeController.lift(data.rightTrigger, remoteController.getTriggerMin(), remoteController.getTriggerMax());
  else
    bladeController.lower(data.leftTrigger, remoteController.getTriggerMin(), remoteController.getTriggerMax());
}


/**
 * @brief Updates all motor systems (wheels and blade) from remtoe controller data.
 */
void Robot::updateMotors(RemoteControllerData data) {
  updateWheelMotors(data);
  updateBladeMotor(data);
}