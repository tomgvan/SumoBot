#ifndef __BLADE_CONTROLLER_H
#define __BLADE_CONTROLLER_H

#include <string>
#include <ESP32Servo.h>


class BladeController {
public:
  BladeController(
    unsigned int pin,
    unsigned int minPulseWidthUs,
    unsigned int maxPulseWidthUs,
    unsigned int freq,
    unsigned int degreesPerSec
  );
  ~BladeController();
  void init();
  void lift(unsigned int val, unsigned int minVal, unsigned int maxVal);
  void lower(unsigned int val, unsigned int minVal, unsigned int maxVal);
  void run();

private:
  //Constants//
  static const std::string kTag;
  const unsigned int kPin;
  const unsigned int kMinPulseWidthUs;
  const unsigned int kMaxPulseWidthUs;
  const unsigned int kFreq;
  const double kDegreesPerUs;

  //Variables//
  double pulseWidthAccumulatorUs;
  unsigned int lastIterationMs;
  Servo servoMotor;

  //Methods//
  double calculatePositionChange(unsigned int val, unsigned int minVal, unsigned int maxVal) const;
  int applyAccumulatedChange(int currentPulseWidthUs);
};

#endif // __BLADE_CONTROLLER_H