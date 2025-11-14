#include "RemoteController.h"
#include <Arduino.h>


// Static Constants Initialization //
const std::string RemoteController::kTag               {"Remote Controller"};
const unsigned int RemoteController::kJoystickDeadzone {30};
const double RemoteController::kMaxJoystickVal         {512.0};
const unsigned int RemoteController::kMinTriggerVal    {0};
const unsigned int RemoteController::kMaxTriggerVal    {1023};


RemoteController::RemoteController(): controller(nullptr) {
  
}


RemoteController::~RemoteController() {
  if(isConnected(controller)) {
    controller->disconnect();
    //todo check if need to set controller to nullptr or not
  }
}


/**
 * @brief Initializes the controller manager.
 * @note This function must be called, typically in setup(), to register
 * connection/disconnection callbacks and enable Bluetooth pairing before any other method is called.
 */
void RemoteController::init() {
  ESP_LOGI(kTag.c_str(), "Firmware: %s", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  ESP_LOGI(kTag.c_str(), "BD Addr: %2X:%2X:%2X:%2X:%2X:%2X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

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
  
  BP32.forgetBluetoothKeys();

  BP32.enableNewBluetoothConnections(true);
}


/**
 * @brief Gets the maximum absolute value for a joystick axis.
 */
double RemoteController::getJoystickMax() const {
  return kMaxJoystickVal;
}


/**
 * @brief Gets the joystick deadzone radius.
 * If the axis magnitude is below this value, it should be treated as 0.
 */
int RemoteController::getJoystickDeadZone() const {
  return kJoystickDeadzone;
}


/**
 * @brief Gets the maximum value of the trigger.
 */
unsigned int RemoteController::getTriggerMax() const {
  return kMaxTriggerVal;
}


/**
 * @brief Gets the minimum value of the trigger.
 */
unsigned int RemoteController::getTriggerMin() const {
  return kMinTriggerVal;
}


/**
 * @brief Updates the controller state.
 * @note This function should be called repeatedly in the main loop.
 * It checks for a connection, polls for new data, and processes gamepad events.
 */
bool RemoteController::run() {
  if(!isConnected(controller))
    return false;
  if(!controller->hasData())
    return false;

  if (controller->isGamepad())
    return processGamepad();

  return false;
}


/**
 * @brief Returns the latest processed data from the connected controller.
 */
RemoteControllerData RemoteController::getData() const {
  if(!isConnected(controller))
    return RemoteControllerData{true, 0, 0, 0, 0};
  
  ESP_LOGV(kTag.c_str(), 
    "Returning joystick data. X: %d | Y: %d | Right trigger: %d | Left trigger: %d",
    controller->axisRX(), 
    controller->axisRY(),
    controller->throttle(),
    controller->brake()
  );

  return RemoteControllerData{
    false, 
    controller->axisRX(), 
    controller->axisRY(),
    static_cast<unsigned int>(controller->throttle()),
    static_cast<unsigned int>(controller->brake())
  };
}


/**
 * @brief Checks if a specific controller pointer is valid and connected.
 */
bool RemoteController::isConnected(ControllerPtr ctl) const {
    return (ctl != nullptr) && (!ctl->isConnected());
}


/**
 * @brief A callback which is called when a controller connects.
 *
 * Enforces a single-controller policy: if no controller is
 * active, it makes the given controller the active one. If one is already active,
 * it disconnects from the controller that just connected.
 */
void RemoteController::onConnectedController(ControllerPtr ctl) {
  ESP_LOGI(kTag.c_str(), "Callback: Controller connected");

  if(controller == nullptr) {
    ESP_LOGV(kTag.c_str(), "Callback: Added controller", kTag.c_str());
    controller = ctl;
  }
  else {
    ESP_LOGV(kTag.c_str(), "Callback: There is already an active controller connected!", kTag.c_str());
    if(ctl != nullptr)
      ctl->disconnect();
  }
}


/**
 * @brief A callback which is called when a controller disconnects.
 *
 * If the disconnecting controller is the one this class is tracking,
 * it sets the internal 'controller' pointer to nullptr.
 */
void RemoteController::onDisconnectedController(ControllerPtr ctl) {
  ESP_LOGI(kTag.c_str(), "Callback: Controller disconnected");

  if(controller == ctl) {
    ESP_LOGV(kTag.c_str(), "Callback: Removed active controller");
    controller = nullptr;
  }
  else {
    ESP_LOGV(kTag.c_str(), "Callback: No active controller found!");
  }
}



/**
 * @brief TODO
 */
bool RemoteController::processGamepad() {
  return BP32.update();
}


/**
 * @brief Prints all available data from a controller to the Serial port.
 */
void RemoteController::dumpGamepad(ControllerPtr ctl) const {
    ESP_LOGV(kTag.c_str(),
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d",
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