#ifndef __BLADE_CONTROLLER_H
#define __BLADE_CONTROLLER_H

#include <string>
#include <ESP32Servo.h>


class BladeController {
public:
  //Methods//
  BladeController(
    unsigned int pin,
    unsigned int minPulseWidthUs,
    unsigned int maxPulseWidthUs,
    unsigned int freq,
    unsigned int degreesPerSec
  );
  ~BladeController();
  void init();
  void lift();
  void lower();
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
  unsigned long lastIterationUs;
  Servo servoMotor;

  //Methods//
  double calculatePositionChange() const;
  int applyAccumulatedChange(int currentPulseWidthUs);
};

#endif // __BLADE_CONTROLLER_H