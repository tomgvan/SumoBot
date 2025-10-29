#include "MotorsController.h"

// Constants
static const int16_t JOYSTICK_DEADZONE = 30;
static const uint8_t MAX_MOTOR_SPEED = 100;
static const uint8_t MIN_MOTOR_SPEED = 0;
static const double MAX_JOYSTICK = 512.0;
static const std::string TAG = "Motors Controller";

MotorsController::MotorsController(uint8_t en_r, uint8_t in1_r, uint8_t in2_r, 
                                        uint8_t en_l, uint8_t in1_l, uint8_t in2_l) 
                                        : motors(en_r, in1_r, in2_r, en_l, in1_l, in2_l) {
                                            
}


void MotorsController::updateMotorsFromInput(int16_t x, int16_t y) {
    inputTranslate(x, y);
}


void MotorsController::run() {
    motors.setSpeedA(state.motorRight.speed);
    motors.setSpeedB(state.motorLeft.speed);

    motors.runForA(state.motorRight.delayMs, state.motorRight.direction);
    motors.runForB(state.motorLeft.delayMs, state.motorLeft.direction);
}


void MotorsController::inputTranslate(int16_t x, int16_t y) {
    int8_t speedRight = 0;
    int8_t speedLeft = 0;
    bool res = inputToSpeed(x, y, speedRight, speedLeft);
    
    state.motorRight.direction = speedToDirection(speedRight);
    state.motorLeft.direction = speedToDirection(speedLeft);

    state.motorRight.speed = abs(speedRight);
    state.motorLeft.speed = abs(speedLeft);

    state.motorRight.delayMs = 5;
    state.motorLeft.delayMs = 5;
}


L298N::Direction MotorsController::speedToDirection(int8_t speed) {
    if(speed > 0)         return L298N::FORWARD;
    else if(speed < 0)    return L298N::BACKWARD;
    return L298N::STOP;
}


bool MotorsController::inputToSpeed(int16_t x, int16_t y, int8_t& motorRight, int8_t& motorLeft) {
    //
    if (hypot(static_cast<double>(x), static_cast<double>(y)) < JOYSTICK_DEADZONE) {
        Serial.printf("%s - Joystick input magnitude inside deadzone. Returning motor speed 0.\n", TAG);
        motorRight = 0;
        motorLeft = 0;
        return false;
    }
    
    //Sperate components and normalize(-1 to +1)
    double throttle = -static_cast<double>(y) / MAX_JOYSTICK;
    double steer = static_cast<double>(x) / MAX_JOYSTICK;
    Serial.printf("%s - Normalized. Throttle: %d | Steer: %d\n", TAG, throttle, steer);

    //Add the components toghter(-2 to +2)
    double rawRight = throttle - steer;
    double rawLeft = throttle + steer;
    Serial.printf("%s - Raw R: %f | Raw L: %f\n", TAG, rawRight, rawLeft);

    //
    double maxMagnitude = max(abs(rawLeft), abs(rawRight));
    double scale = 1.0;
    if (maxMagnitude > 1.0)
        scale = 1.0 / maxMagnitude;
    // log(VERBOSE, TAG, "Speed scale: " + scale);

    //Finalize motors speed(0 to 100)
    motorRight = (rawRight * scale) * MAX_MOTOR_SPEED;
    motorLeft = (rawLeft * scale) * MAX_MOTOR_SPEED;
    // log(INFO, TAG, "R Speed: " + motorRight + " L Speed: " + motorLeft);

    return true;
}