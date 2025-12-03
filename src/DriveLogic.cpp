#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "../include/DriveLogic.h"
#include "../include/HBridgeMotor.h"


// Static Constants Initialization //
const std::string DriveLogic::kTag {"Drive Logic"};


DriveLogic::DriveLogic(
    double joystickMaxVal, 
    unsigned int joystickDeadzone, 
    unsigned int maxMotorSpeed) :
        kJoystickMaxVal(joystickMaxVal),
        kJoystickDeadzone(joystickDeadzone),
        kMaxMotorSpeed(maxMotorSpeed) {
  
}


/**
 * @brief Translates joystick (x, y) into final motor speeds and directions.
 *
 * Calls 'joystickToSpeed' to get signed speeds, then converts them
 * into unsigned speeds (0 to kMaxMotorSpeed) and 'HBridgeMotor::Direction'
 * enums for output.
 */
void DriveLogic::handleJoystickInput(
                          int x, 
                          int y, 
                          unsigned int& speedR, 
                          unsigned int& speedL, 
                          HBridgeMotor::Direction& directionR, 
                          HBridgeMotor::Direction& directionL
                        ) const {
  int signedSpeedR {0};
  int signedSpeedL {0};
  joystickToSpeed(x, y, signedSpeedR, signedSpeedL);

  directionR = speedToDirection(signedSpeedR);
  directionL = speedToDirection(signedSpeedL);

  speedR = std::abs(signedSpeedR);
  speedL = std::abs(signedSpeedL);
  
  ESP_LOGV(kTag.c_str(), "x: %d | y: %d | SpeedR: %u | SpeedL: %u | DirectionR: %d | DirectionL: %d", x, y, speedR, speedL, directionR, directionL);
}


/**
 * @brief Converts a signed speed value into a motor direction enum(Forward/Backward/Stop).
 */
 HBridgeMotor::Direction DriveLogic::speedToDirection(int speed) const {
    if(speed > 0)         return HBridgeMotor::Direction::kForward;
    else if(speed < 0)    return HBridgeMotor::Direction::kBackward;
    return HBridgeMotor::Direction::kStop;
}


/**
 * @brief Checks if joystick (x, y) is in the circular deadzone 'kJoystickDeadzone'.
 * 
 * Returns true if inside the deadzone and false otherwise.
 */
bool DriveLogic::isInsideDeadzone(int x, int y) const {
    if (std::hypot(static_cast<double>(x), static_cast<double>(y)) < kJoystickDeadzone) {
        ESP_LOGV(kTag.c_str(), "Joystick input magnitude inside deadzone. Returning motor speed 0.");
        return true;
    }

    return false;
}


/**
 * @brief Sets the right and left motor speed for a point turn. Motor speeds are 
 *  equal in magnitude but oppisite in direction.
 */
void DriveLogic::setPointTurnUnscaled(double steer, double& unscaledR, double& unscaledL) const {
    ESP_LOGV(kTag.c_str(), "Mode: Point Turn");
    unscaledR = -steer;
    unscaledL = steer;
}


/**
 * @brief Sets the right and left motor speed to the provided throttle. Resulting in a straight movement.
 */
void DriveLogic::setStraightUnscaled(double throttle, double& unscaledR, double& unscaledL) const {
    ESP_LOGV(kTag.c_str(), "Mode: Straight");
    unscaledR = throttle;
    unscaledL = throttle;
}


/**
 * @brief Calculates the right and left wheel speed for an arc turn using blended throttle and steer inputs.
 *
 * Sets the wheel speeds for an arc turn (forward/backward movement while turning). 
 * It calculates the outer wheel speed based on the inputs and then scales the inner wheel speed using a ratio. 
 * This ratio is determined by the turn angle. The closer the angle is to 90 degrees (a point turn), 
 * the slower the inner wheel, and the closer it is to 0 degrees (straight), the closer the speeds are.
 */
void DriveLogic::setArcTurnUnscaled(double throttle, 
                                    double steer,  
                                    double& unscaledR, 
                                    double& unscaledL) const {
    const double magnitude              {std::hypot(steer, throttle)};
    const double turnAngleRad           {std::atan2(steer, throttle)};
    const double outerWheelSpeed        {throttle < 0.0 ? -magnitude : magnitude};

    //[0.0 - 1.0] The closer the x & y to straight forward/backward the closer it is to 1.0
    //The closer the x & y to point turn the closer it is to 0.0
    const double innerWheelSpeedRatio   {std::abs(1.0 - (std::abs(turnAngleRad) / HALF_PI))};

    ESP_LOGV(kTag.c_str(), "Mode: Arc Turn. | base speed: %f | angle(rad): %f | turn ratio: %f", outerWheelSpeed, turnAngleRad, innerWheelSpeedRatio);

    if (steer > 0.0) {
        //Turning right -> Left wheel is Outer wheel
        unscaledL = outerWheelSpeed;
        unscaledR = outerWheelSpeed * innerWheelSpeedRatio;
    } else {
        //Turning left -> Right wheel is Outer wheel
        unscaledR = outerWheelSpeed;
        unscaledL = outerWheelSpeed * innerWheelSpeedRatio;
    }
}


/**
 * @brief Calculates differential steering motor speeds while maximizing motors torque when turning.
 *
 * 
 */
void DriveLogic::calcDifferentialSpeed(int x, int y, double& unscaledR, double& unscaledL) const {
    //TODO adjust values to the circular deadzone
    const double throttleDeadzone   {static_cast<double>(kJoystickDeadzone) / kJoystickMaxVal};
    const double steer              {static_cast<double>(x) / kJoystickMaxVal};
    const double throttle           {(-static_cast<double>(y)) / kJoystickMaxVal};
    ESP_LOGV(kTag.c_str(), "Normalized. Throttle: %f | Steer: %f", throttle, steer);

    if (std::abs(throttle) < throttleDeadzone)
        setPointTurnUnscaled(steer, unscaledR, unscaledL);
    else if(steer == 0.0)
        setStraightUnscaled(throttle, unscaledR, unscaledL);
    else
        setArcTurnUnscaled(throttle, steer, unscaledR, unscaledL);

    ESP_LOGV(kTag.c_str(), "Unscaled R: %f | Unscaled L: %f", unscaledR, unscaledL);
}



/**
 * @brief Calculates a factor (<= 1.0) to cap the max motor speed
 * at 1.0 while preserving the steering ratio.
 */
double DriveLogic::calcNormalizationFactor(double unscaledR, double unscaledL) const {
    const double maxMagnitude {std::max(std::abs(unscaledR), std::abs(unscaledL))};
    double factor {1.0};
    if (maxMagnitude > 1.0)
        factor = 1.0 / maxMagnitude;

    return factor;
}


/**
 * @brief Converts joystick (x, y) coordinates to final motor speeds.
 *
 * Applies the deadzone, calculates the differential steering mix, and proportionally scales
 * the output to the [-kMaxMotorSpeed, +kMaxMotorSpeed] range.
 */
void DriveLogic::joystickToSpeed(int x, int y, int& speedR, int& speedL) const {
    if (isInsideDeadzone(x, y))
        return;

    double unscaledRight {};
    double unscaledLeft {};
    calcDifferentialSpeed(x, y, unscaledRight, unscaledLeft);

    const double factor {calcNormalizationFactor(unscaledRight, unscaledLeft)};

    //Finalize motors speed(0 to kMaxMotorSpeed)
    speedR = (unscaledRight * factor) * kMaxMotorSpeed;
    speedL = (unscaledLeft * factor) * kMaxMotorSpeed;
}