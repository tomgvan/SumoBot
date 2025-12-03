#ifndef __H_BRIDGE_MOTOR_H
#define __H_BRIDGE_MOTOR_H

#include <string>


class HBridgeMotor {
public:
  //Constants//
  static constexpr unsigned int kMinMotorSpeed {0};
  static constexpr unsigned int kMaxMotorSpeed {255};
  typedef enum
  {
      kForward = 0,
      kBackward = 1,
      kStop = -1
  } Direction;

  //Methods//
  HBridgeMotor(unsigned int en, unsigned int in1, unsigned int in2);
  ~HBridgeMotor();
  void init();
  void setSpeed(unsigned int speed);
  void setDirection(Direction direction);


private:
  //Constants//
  static const std::string kTag;
  const unsigned int kEn;
  const unsigned int kIn1;
  const unsigned int kIn2;
};

#endif // __H_BRIDGE_MOTOR_H