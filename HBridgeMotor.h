#ifndef __H_BRIDGE_H
#define __H_BRIDGE_H

#include <stdint.h>
#include <string>


class HBridgeMotor {
public:
  typedef enum
  {
      kForward = 0,
      kBackward = 1,
      kStop = -1
  } Direction;

  HBridgeMotor(uint8_t en, uint8_t in1, uint8_t in2);
  ~HBridgeMotor();
  void init() const;
  void setSpeed(uint8_t speed) const;
  void setDirection(Direction direction) const;


private:
  //Constants//
  static const std::string kTag;
  const uint8_t kEn;
  const uint8_t kIn1;
  const uint8_t kIn2;
};

#endif