#include "HBridgeMotor.h"
#include "esp32-hal-gpio.h"


// Static Constants Initialization //
const std::string HBridgeMotor::kTag {"H-Bridge Motor"};


HBridgeMotor::HBridgeMotor(
    unsigned int en, 
    unsigned int in1, 
    unsigned int in2):
      kEn(en), 
      kIn1(in1),
      kIn2(in2) {

}


HBridgeMotor::~HBridgeMotor() {
  setDirection(HBridgeMotor::Direction::kStop);
  setSpeed(0);
}


/**
 * @brief Initializes the H-Bridge motor controller pins.
 *
 * @note This function must be called (e.g., in your setup() function)
 * before using any other motor functions.
 *
 * Sets the EN, IN1, and IN2 pins to OUTPUT mode and the initial motor state to stopped.
 */
void HBridgeMotor::init() {
  pinMode(kEn, OUTPUT);
  pinMode(kIn1, OUTPUT);
  pinMode(kIn2, OUTPUT);
  
  setDirection(HBridgeMotor::Direction::kStop);
  setSpeed(0);
}


/**
 * @brief Sets the speed of the motor using PWM(higher value -> higher speed).
 */
void HBridgeMotor::setSpeed(unsigned int speed) {
  //TODO check value boundaries
  analogWrite(kEn, speed);
}


/**
 * @brief Sets the motor's direction of rotation.
 *
 * Controls the IN1 and IN2 pins to set the motor to forward, backward, or stop.
 */
void HBridgeMotor::setDirection(HBridgeMotor::Direction direction) {
  if(direction == HBridgeMotor::Direction::kForward) {
    digitalWrite(kIn1, HIGH);
    digitalWrite(kIn2, HIGH);
  }
  else if(direction == HBridgeMotor::Direction::kBackward) {
    digitalWrite(kIn1, HIGH);
    digitalWrite(kIn2, LOW);
  }
  else {
    digitalWrite(kIn1, LOW);
    digitalWrite(kIn2, LOW);
  }
}