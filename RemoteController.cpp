#include "RemoteController.h"
#include <Arduino.h>
#include <string>


// Static Constants Initialization
const std::string RemoteController::kTag = "Remote Controller";
const unsigned int RemoteController::kJoystickDeadzone = 30;
const double RemoteController::kMaxJoystickVal = 512.0;


RemoteController::RemoteController(): controller(nullptr) {
  
}


RemoteController::~RemoteController() {
  if(isConnected(controller)) {
    controller->disconnect();
    //todo check if need to set controller to nullptr or not
  }
}


//Initialize the remote controller callbacks and allows new remote controllers to connect
//Must be called on Startup!
void RemoteController::init() {
  BP32.setup(
    // Lambda function for onConnect
    [this](ControllerPtr controller) {
        this->onConnectedController(controller);
    },
    
    // Lambda function for onDisconnect
    [this](ControllerPtr controller) {
        this->onDisconnectedController(controller);
    }
  );
  
  BP32.enableNewBluetoothConnections(true);
}


//Returns the joystick max positive value
double RemoteController::getJoystickMax() {
  return kMaxJoystickVal;
}


//Returns the joystick deadzone
//If the axises magnitude is below the deadzone the user might want to regard it as 0
int RemoteController::getJoystickDeadZone() const {
  return kJoystickDeadzone;
}


//Requests to update the connected controller data
//Returns true if data updated and ready to be read and false if no update occured
bool RemoteController::run() {
  if(!isConnected(controller))
    return false;
  if(!controller->hasData())
    return false;

  if (controller->isGamepad())
    return processGamepad();

  return false;
}


//Returns the latest right joystick data(x & y) if a controller is connected
//Otherwise returns an error indication and (x=0 & y=0)
RemoteControllerData RemoteController::getData() {
  if(!isConnected(controller))
    return RemoteControllerData{true, 0, 0};
  
  Serial.printf("%s - Returning joystick data. X: %d | Y: %d\n", kTag.c_str(), controller->axisRX(), controller->axisRY());
  return RemoteControllerData{false, controller->axisRX(), controller->axisRY()};
}


//Returns true if there is a controller connected and false otherwise
bool RemoteController::isConnected(ControllerPtr ctl) {
    return (ctl != nullptr) && (!ctl->isConnected());
}


//A callback which is called when a remote controller is connected
void RemoteController::onConnectedController(ControllerPtr ctl) {
  Serial.printf("%s - Callback: Controller connected\n", kTag.c_str());

  if(controller == nullptr) {
    Serial.printf("%s - Callback: Added controller\n", kTag.c_str());
    controller = ctl;
  }
  else {
    Serial.printf("%s - Callback: There is already an active controller connected!\n", kTag.c_str());
    if(ctl != nullptr)
      ctl->disconnect();
  }
}


//A callback which is called when a remote controller is disconnected
void RemoteController::onDisconnectedController(ControllerPtr ctl) {
  Serial.printf("%s - Callback: Controller disconnected\n", kTag.c_str());

  if(controller == ctl) {
    Serial.printf("%s - Callback: Removed active controller\n", kTag.c_str());
    controller = nullptr;
  }
  else {
    Serial.printf("%s - Callback: No active controller found!\n", kTag.c_str());
  }
}


//Requests to update the controller data
//Returns true if data updated and ready to be read and false if not
bool RemoteController::processGamepad() {
  return BP32.update();
}


//Prints the connected remote gamepad data
void RemoteController::dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "%s - idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        kTag.c_str(),
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}