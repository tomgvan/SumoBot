#ifndef __H_BRIDGE_H
#define __H_BRIDGE_H

#include <string>


class HBridgeMotor {
public:
  typedef enum
  {
      kForward = 0,
      kBackward = 1,
      kStop = -1
  } Direction;

  HBridgeMotor(unsigned int en, unsigned int in1, unsigned int in2);
  ~HBridgeMotor();
  void init() const;
  void setSpeed(unsigned int speed) const;
  void setDirection(Direction direction) const;


private:
  //Constants//
  static const std::string kTag;
  const unsigned int kEn;
  const unsigned int kIn1;
  const unsigned int kIn2;
};

#endif