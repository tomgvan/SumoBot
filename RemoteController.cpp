#include "RemoteController.h"


static const std::string TAG = "Remote Controller";


RemoteController::RemoteController(): controller(nullptr) {
  BP32.setup([this](ControllerPtr ctl) { this->onConnectedController(ctl); },
              [this](ControllerPtr ctl) { this->onDisconnectedController(ctl); });
  BP32.enableNewBluetoothConnections(true);
}


bool RemoteController::run() {
  if(!isConnected(controller))
    return false;
  if(!controller->hasData())
    return false;

  if (controller->isGamepad())
    return processGamepad();

  return false;
}


RemoteControllerData RemoteController::getData() {
  if(!isConnected(controller))
    return RemoteControllerData{true, 0, 0};
  
  Serial.printf("%s - Returning joystick data. X: %d | Y: %d\n", TAG, controller->axisRX(), controller->axisRY());
  return RemoteControllerData{false, controller->axisRX(), controller->axisRY()};
}


bool RemoteController::isConnected(ControllerPtr ctl) {
    return (ctl != nullptr) && (!ctl->isConnected());
}


void RemoteController::onConnectedController(ControllerPtr ctl) {
  Serial.printf("%s - Callback: Controller connected\n", TAG);

  if(controller == nullptr) {
    Serial.printf("%s - Callback: Added controller\n", TAG);
    controller = ctl;
  }
  else {
    Serial.printf("%s - Callback: There is already an active controller connected!\n", TAG);
  }
}


void RemoteController::onDisconnectedController(ControllerPtr ctl) {
  Serial.printf("%s - Callback: Controller disconnected\n", TAG);

  if(controller == ctl) {
    Serial.printf("%s - Callback: Removed active controller\n", TAG);
    controller = nullptr;
  }
  else {
    Serial.printf("%s - Callback: No active controller found!\n", TAG);
  }
}


bool RemoteController::processGamepad() {
  return BP32.update();
}


void RemoteController::dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "%s - idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        TAG,
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