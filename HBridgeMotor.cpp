#include "HBridgeMotor.h"
#include "esp32-hal-gpio.h"


// Static Constants Initialization
const std::string HBridgeMotor::kTag = "H-Bridge Motor";


HBridgeMotor::HBridgeMotor(uint8_t en, uint8_t in1, uint8_t in2):
                                      kEn(en), kIn1(in1), kIn2(in2) {

}


HBridgeMotor::~HBridgeMotor() {
  setDirection(HBridgeMotor::Direction::kStop);
  setSpeed(0);
}


//Initializes the 3 gpios as output and setting the motor speed to 0(stop)
//Must be called on startup!
void HBridgeMotor::init() const {
  pinMode(kEn, OUTPUT);
  pinMode(kIn1, OUTPUT);
  pinMode(kIn2, OUTPUT);
  
  setDirection(HBridgeMotor::Direction::kStop);
  setSpeed(0);
}


//Sets the speed of the motor which is connected via H-Bridge 0(stall) to 255(fastest)
void HBridgeMotor::setSpeed(uint8_t speed) const {
  analogWrite(kEn, speed);
}


//Sets the direction of the motor which is connected via H-Bridge stop/forward/backward
void HBridgeMotor::setDirection(HBridgeMotor::Direction direction) const {
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