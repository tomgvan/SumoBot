#include "BladeController.h"
#include <math.h>
#include <Arduino.h>


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
      lastIterationMs(millis()) {

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
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

	servoMotor.setPeriodHertz(kFreq);
	servoMotor.attach(kPin, kMinPulseWidthUs, kMaxPulseWidthUs);
}


/**
 * @brief Calculates position change (in degrees) based on input and time.
 *
 * Normalizes the input `val` to a speed factor [0.0, 1.0] using
 * `minVal` and `maxVal`. It then scales this speed by `kDegreesPerUs`
 * and the elapsed time to get a consistent movement delta.
 */
double BladeController::calculatePositionChange(unsigned int val, unsigned int minVal, unsigned int maxVal) const {
  val = constrain(val, minVal, maxVal);

  double elapsedMicroseconds {(millis() - lastIterationMs) * 1000.0};
  double normalizedSpeed {(val - minVal) / static_cast<double>(maxVal - minVal)};//0.0 - 1.0

  return elapsedMicroseconds * (kDegreesPerUs * normalizedSpeed);
}


/**
 * @brief Queues a "lift" (extend) movement.
 *
 * Calculates the desired position change and adds it to the
 * accumulator, causing the next calls to `run()` to move towards the max limit.
 */
void BladeController::lift(unsigned int val, unsigned int minVal, unsigned int maxVal) {
  double positionChangeUs {calculatePositionChange(val, minVal, maxVal)};

  if(pulseWidthAccumulatorUs <= 0.0)
    pulseWidthAccumulatorUs = positionChangeUs;
  else
    pulseWidthAccumulatorUs += positionChangeUs;
}


/**
 * @brief Queues a "lower" (retract) movement.
 *
 * Calculates the desired position change and subtracts it from the
 * accumulator, causing the next calls to `run()` to move towards the min limit.
 */
void BladeController::lower(unsigned int val, unsigned int minVal, unsigned int maxVal) {
  double positionChangeUs {calculatePositionChange(val, minVal, maxVal)};

  if(pulseWidthAccumulatorUs >= 0.0)
    pulseWidthAccumulatorUs = -positionChangeUs;
  else
    pulseWidthAccumulatorUs -= positionChangeUs;
}


/**
 * @brief Applies the accumulated change to the provided current pulse width.
 *
 * Consumes the integer part of `pulseWidthAccumulatorUs`, adds it
 * to `currentPulseWidthUs`, and keeps the fractional part in the
 * accumulator for the next loop.
 */
int BladeController::applyAccumulatedChange(int currentPulseWidthUs) {
  double changeToApplyUs {trunc(pulseWidthAccumulatorUs)};
  pulseWidthAccumulatorUs -= changeToApplyUs;

  int newPulseWidthUs {currentPulseWidthUs + static_cast<int>(changeToApplyUs)};
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
  lastIterationMs = millis();
  if(pulseWidthAccumulatorUs < 1.0 && pulseWidthAccumulatorUs > -1.0)
    return;

  int currentPulseWidthUs {servoMotor.readMicroseconds()};
  int newPulseWidthUs {applyAccumulatedChange(currentPulseWidthUs)};
  
  servoMotor.writeMicroseconds(newPulseWidthUs);
}