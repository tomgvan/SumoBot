#include <Arduino.h>
#include <cmath>
#include "../include/BladeController.h"


// Static Constants Initialization //
const std::string BladeController::kTag {"Blade Controller"};


BladeController::BladeController(
    unsigned int pin,
    unsigned int minPulseWidthUs,
    unsigned int maxPulseWidthUs,
    unsigned int freq,
    unsigned int degreesPerSec):
      kPin(pin), 
      kMinPulseWidthUs(minPulseWidthUs),
      kMaxPulseWidthUs(maxPulseWidthUs),
      kFreq(freq),
      kDegreesPerUs(degreesPerSec / 1000.0 / 1000.0),
      lastIterationUs(micros()) {

}


BladeController::~BladeController() {
  servoMotor.detach();
}


/**
 * @brief Initializes the blade servo motor.
 * @note Must be called before any other methods in this class (e.g., in setup()).
 *
 * Attaches the servo to its pin and sets its operating
 * frequency and pulse width (movement) range.
 */
void BladeController::init() {
	ESP32PWM::allocateTimer(0);

	servoMotor.setPeriodHertz(kFreq);
	servoMotor.attach(kPin, kMinPulseWidthUs, kMaxPulseWidthUs);
}


/**
 * @brief Calculates position change (in degrees) based on input and time.
 *
 * Determines the distance (in degrees) the blade should have traveled since the last call, based on the
 * elapsed time and the maximum configured angular speed of the servo (`kDegreesPerUs`). 
 * This calculated value represents the step size to be applied to the accumulator add for lifting and subtract for lowering 
 * enabling smooth, continuous motion control. Returns 0.0 if a microcontroller time overflow or excessive time gap is detected.
 */
double BladeController::calculatePositionChange() const {
  constexpr unsigned long kMaxDeltaUs {5 * 60 * 1000 * 1000};//5 Minutes
  const unsigned long currentMicroseconds {micros()};

  //Handle overflow
  if(currentMicroseconds < lastIterationUs)
    return 0.0;
  if(currentMicroseconds - lastIterationUs > kMaxDeltaUs)
    return 0.0;

  const double elapsedMicroseconds {static_cast<double>(currentMicroseconds - lastIterationUs)};
  const double positionChange {elapsedMicroseconds * kDegreesPerUs};
  ESP_LOGV(kTag.c_str(), "elapsed time(us): %f | position change: %f", elapsedMicroseconds, positionChange);

  return positionChange;
}


/**
 * @brief Queues a "lift" (extend) movement.
 *
 * Calculates the desired position change and adds it to the
 * accumulator, causing the next calls to `run()` to move towards the max limit.
 */
void BladeController::lift() {
  const double positionChangeUs {calculatePositionChange()};

  if(pulseWidthAccumulatorUs <= 0.0)
    pulseWidthAccumulatorUs = positionChangeUs;
  else
    pulseWidthAccumulatorUs += positionChangeUs;
  ESP_LOGV(kTag.c_str(), "Lift. pulseWidthAccumulatorUs: %f", pulseWidthAccumulatorUs);
}


/**
 * @brief Queues a "lower" (retract) movement.
 *
 * Calculates the desired position change and subtracts it from the
 * accumulator, causing the next calls to `run()` to move towards the min limit.
 */
void BladeController::lower() {
  const double positionChangeUs {calculatePositionChange()};

  if(pulseWidthAccumulatorUs >= 0.0)
    pulseWidthAccumulatorUs = -positionChangeUs;
  else
    pulseWidthAccumulatorUs -= positionChangeUs;
  ESP_LOGV(kTag.c_str(), "Lower. pulseWidthAccumulatorUs: %f", pulseWidthAccumulatorUs);
}


/**
 * @brief Applies the accumulated change to the provided current pulse width.
 *
 * Consumes the integer part of `pulseWidthAccumulatorUs`, adds it
 * to `currentPulseWidthUs`, and keeps the fractional part in the
 * accumulator for the next loop.
 */
int BladeController::applyAccumulatedChange(int currentPulseWidthUs) {
  const double changeToApplyUs {std::trunc(pulseWidthAccumulatorUs)};
  const int newPulseWidthUs {currentPulseWidthUs + static_cast<int>(changeToApplyUs)};
  pulseWidthAccumulatorUs -= changeToApplyUs;

  return constrain(newPulseWidthUs, kMinPulseWidthUs, kMaxPulseWidthUs);
}


/**
 * @brief Updates the servo's position based on the accumulated change.
 * @note This function MUST be called periodically from the main loop.
 *
 * Reads the servo's current position, calculates the new position by
 * applying the accumulated change, and writes the new value back.
 */
void BladeController::run() {
  lastIterationUs = micros();
  if(pulseWidthAccumulatorUs < 1.0 && pulseWidthAccumulatorUs > -1.0)
    return;

  const int currentPulseWidthUs {servoMotor.readMicroseconds()};
  const int newPulseWidthUs {applyAccumulatedChange(currentPulseWidthUs)};
  ESP_LOGE(kTag.c_str(), "Current pulse width(us): %d | New pulse width(us): %d | Pulse width accumulator(us): %f", 
                                                        currentPulseWidthUs, newPulseWidthUs, pulseWidthAccumulatorUs);

  servoMotor.writeMicroseconds(newPulseWidthUs);
}