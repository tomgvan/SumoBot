#include "esp32-hal-gpio.h"
#include "../include/HBridgeMotor.h"


// Static Constants Initialization //
const std::string HBridgeMotor::kTag                {"H-Bridge Motor"};


HBridgeMotor::HBridgeMotor(
    const HBridgeMotorPins& pins,
    unsigned int minMotorSpeed,
    unsigned int maxMotorSpeed):
      kEn(pins.en),
      kIn1(pins.in1),
      kIn2(pins.in2),
      kMinMotorSpeed(minMotorSpeed),
      kMaxMotorSpeed(maxMotorSpeed) {

}


HBridgeMotor::~HBridgeMotor() {
  setDirection(Direction::kStop);
  setSpeed(kMinMotorSpeed);
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
  
  setDirection(Direction::kStop);
  setSpeed(kMinMotorSpeed);
}


/**
 * @brief Sets the speed of the motor using PWM(higher value -> higher speed).
 */
void HBridgeMotor::setSpeed(unsigned int speed) {
  if(speed > kMaxMotorSpeed)
    speed = kMaxMotorSpeed;

  else if(speed < kMinMotorSpeed)
    speed = kMinMotorSpeed;

  analogWrite(kEn, speed);
}


/**
 * @brief Sets the motor's direction of rotation.
 *
 * Controls the IN1 and IN2 pins to set the motor to forward, backward, or stop.
 */
void HBridgeMotor::setDirection(Direction direction) {
  if(direction == Direction::kForward) {
    digitalWrite(kIn1, HIGH);
    digitalWrite(kIn2, LOW);
  }
  else if(direction == Direction::kBackward) {
    digitalWrite(kIn1, LOW);
    digitalWrite(kIn2, HIGH);
  }
  else {
    digitalWrite(kIn1, LOW);
    digitalWrite(kIn2, LOW);
  }
}


/**
 * @brief Returns the minimum motor speed(stop speed).
 */
unsigned int HBridgeMotor::getMinSpeed() {
  return kMinMotorSpeed;
}