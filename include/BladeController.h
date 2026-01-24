#pragma once

#include <ESP32Servo.h>
#include "Gpios.h"


struct BladeControllerConfig {
  unsigned int pin;
  unsigned int minPulseWidthUs;
  unsigned int maxPulseWidthUs;
  unsigned int freq;
  unsigned int degreesPerSec;
};


class BladeController {
public:
  //Constants//
  static constexpr BladeControllerConfig kDefaultConfig {
    .pin              = gpios::kBladeMotorPin,
    .minPulseWidthUs  = 750,
    .maxPulseWidthUs  = 2250,
    .freq             = 50,
    .degreesPerSec    = 300
  };

  //Methods//
  explicit BladeController(const BladeControllerConfig& bladeConfig = kDefaultConfig);
  ~BladeController();
  void init();
  void lift();
  void lower();
  void run();

private:
  //Constants//
  static constexpr const char* kTag {"Blade Controller"};
  const unsigned int kPin;
  const unsigned int kFreq;
  const double kDegreesPerUs;
  const unsigned int kMinPulseWidthUs;
  const unsigned int kMaxPulseWidthUs;

  //Variables//
  double pulseWidthAccumulatorUs;
  unsigned long lastIterationUs;
  Servo servoMotor;

  //Methods//
  double calculatePositionChange() const;
  int applyAccumulatedChange(int currentPulseWidthUs);
};