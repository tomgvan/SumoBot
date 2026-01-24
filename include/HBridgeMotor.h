#ifndef __H_BRIDGE_MOTOR_H
#define __H_BRIDGE_MOTOR_H

#include <string>


struct HBridgeMotorPins {
  unsigned int en;
  unsigned int in1;
  unsigned int in2;
};


class HBridgeMotor {
public:
  enum class Direction {
    kForward    = 1,
    kBackward   = -1,
    kStop       = 0
  };

  //Methods//
  HBridgeMotor(
    const HBridgeMotorPins& pins,
    unsigned int minMotorSpeed,
    unsigned int maxMotorSpeed
  );
  ~HBridgeMotor();
  void init();
  void setSpeed(unsigned int speed);
  void setDirection(Direction direction);
  unsigned int getMinSpeed();


private:
  //Constants//
  static const std::string kTag;
  const unsigned int kEn;
  const unsigned int kIn1;
  const unsigned int kIn2;
  const unsigned int kMinMotorSpeed;
  const unsigned int kMaxMotorSpeed;
};

#endif // __H_BRIDGE_MOTOR_H