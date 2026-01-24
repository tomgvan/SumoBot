#include <Arduino.h>
#include "../include/RemoteController.h"


// Static Constants Initialization //
constexpr const char* RemoteController::kTag;


RemoteController::RemoteController(): controller(nullptr), disconnectCallback(nullptr) {
  
}


RemoteController::~RemoteController() {
  if(isConnected(controller)) {
    controller->disconnect();
    //todo check if need to set controller to nullptr or not
    //todo check if need to set disconnectCallback to nullptr and where
  }
}


/**
 * @brief Initializes the remote controller manager.
 * @note This function must be called, typically in setup(), to register
 * connection/disconnection callbacks and enable Bluetooth pairing before any other method is called.
 */
void RemoteController::init() {
  ESP_LOGI(kTag, "Initializing RemoteController.\n");

  ESP_LOGV(kTag, "Firmware: %s", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  ESP_LOGV(kTag, "BD Addr: %2X:%2X:%2X:%2X:%2X:%2X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  
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
  
  // BP32.forgetBluetoothKeys();

  BP32.enableNewBluetoothConnections(true);
  
  BP32.enableVirtualDevice(false);

  changeTxPower(ESP_PWR_LVL_P9);
}


/**
 * @brief Updates the controller state.
 * @note This function should be called repeatedly in the main loop.
 * It checks for a connection, polls for new data, and processes gamepad events.
 */
bool RemoteController::run() {
  return BP32.update();
}


/**
 * @brief Returns the latest processed data from the connected controller.
 */
RemoteControllerData RemoteController::getData() const {
  if(!isConnected(controller))
    return RemoteControllerData{true, 0, 0, 0, 0};
  if(!controller->isGamepad())
    return RemoteControllerData{true, 0, 0, 0, 0};

  ESP_LOGV(kTag, 
    "Returning joystick data. X: %d | Y: %d | Right trigger: %d | Left trigger: %d",
    controller->axisX(), 
    controller->axisY(),
    controller->throttle(),
    controller->brake()
  );

  return RemoteControllerData{
    false, 
    controller->axisX(), 
    controller->axisY(),
    static_cast<unsigned int>(controller->throttle()),
    static_cast<unsigned int>(controller->brake()),
    controller->buttons() & kRightButtonMask,
    controller->buttons() & kLeftButtonMask
  };
}


/**
 * @brief Checks if a specific controller pointer is valid and connected.
 */
bool RemoteController::isConnected(ControllerPtr ctl) const {
  return (ctl != nullptr) && (ctl->isConnected());
}


/**
 * @brief A callback which is called when a controller connects.
 *
 * Enforces a single-controller policy: if no controller is
 * active, it makes the given controller the active one. If one is already active,
 * it disconnects from the controller that just connected.
 */
void RemoteController::onConnectedController(ControllerPtr ctl) {
  ESP_LOGI(kTag, "Callback: Controller connected");

  if(controller == nullptr) {
    ESP_LOGI(kTag, "Callback: Added controller");
    controller = ctl;
  }
  else {
    ESP_LOGI(kTag, "Callback: There is already an active controller connected!");
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
  //TODO on disconnect stop all motors
  ESP_LOGI(kTag, "Callback: Controller disconnected");

  if(controller == ctl) {
    ESP_LOGV(kTag, "Callback: Removed active controller");
    controller = nullptr;

    if(disconnectCallback != nullptr)
      disconnectCallback();
  }
  else {
    ESP_LOGW(kTag, "Callback: No active controller found!");
  }
}


/**
 * @brief Prints all available data from a controller to the Serial port.
 */
void RemoteController::dumpGamepad(ControllerPtr ctl) const {
    ESP_LOGV(kTag,
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


/**
 * @brief Setting the provided disconnect callback to the provided one.
 * 
 * The provided callback will be called whenever a remote controller is disconnected.
 */
void RemoteController::registerDisconnectCallback(const ControllerDisconnectCallback &callback) {
  disconnectCallback = callback;
}


/**
 * @brief Changes the Bluetooth Low Energy and Classic Bluetooth transmittion power.
 */
void RemoteController::changeTxPower(esp_power_level_t pwrLvl) {
  esp_power_level_t bredrMinTxLvl {};
  esp_power_level_t bredrMaxTxLvl {};
  esp_power_level_t bleTxLvl {};
  esp_err_t err {};

  //Reading current Bluetooth tx power
  bleTxLvl = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT);
  if(bleTxLvl < 0) {
    ESP_LOGW(kTag, "BLE - Couldnt read current Tx lvl! %d", bleTxLvl);

  } else {
    ESP_LOGI(kTag, "BLE - Current TX lvl: %d", powerLvlToDbm(bleTxLvl));
  }

  err = esp_bredr_tx_power_get(&bredrMinTxLvl, &bredrMaxTxLvl);
  if(err != ESP_OK) {
    ESP_LOGW(
      kTag, "BREDR - Couldnt read current Tx lvl! 0x%x", err);
  }
  else {
    ESP_LOGI(
      kTag, 
      "BREDR - Current Tx lvl. min: %d | max: %d", 
      powerLvlToDbm(bredrMinTxLvl), 
      powerLvlToDbm(bredrMaxTxLvl)
    );
  }

  //Changing Bluetooth Low Energy tx power
  err = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, pwrLvl);
  if(err != ESP_OK) {
    ESP_LOGW(kTag, "BLE - Couldn't change ble tx power lvl! 0x%x", err);
  } else{
    ESP_LOGI(kTag, "BLE - Changed Tx power lvl to: %d", powerLvlToDbm(pwrLvl));  
  }

  //Changing Bluetooth Classic tx power
  err = esp_bredr_tx_power_set(pwrLvl, pwrLvl);
  if(err != ESP_OK) {
    ESP_LOGW(kTag, "BREDR - Couldn't change bredr tx power lvl! 0x%x", err);
  } else {
    ESP_LOGI(kTag, "BREDR - Changed Tx power lvl to: %d", powerLvlToDbm(pwrLvl));
  }
}


/**
 * @brief Converts the BT stack power level enum values to dbm.
 */
int RemoteController::powerLvlToDbm(esp_power_level_t pwrLvl) const {
  if(pwrLvl == ESP_PWR_LVL_N12) return -12;
  if(pwrLvl == ESP_PWR_LVL_N9)  return -9;
  if(pwrLvl == ESP_PWR_LVL_N6)  return -6;
  if(pwrLvl == ESP_PWR_LVL_N3)  return -3;
  if(pwrLvl == ESP_PWR_LVL_N0)  return 0;
  if(pwrLvl == ESP_PWR_LVL_P3)  return 3;
  if(pwrLvl == ESP_PWR_LVL_P6)  return 6;
  if(pwrLvl == ESP_PWR_LVL_P9)  return 9;
  return -100;
}