#ifndef __MOTORS_CONTROLLER_H
#define __MOTORS_CONTROLLER_H

#include <stdint.h>
#include <cmath>
#include <algorithm>
#include <L298NX2.h>


class MotorsController {
public:
    MotorsController(uint8_t en_r, uint8_t in1_r, uint8_t in2_r, 
                    uint8_t en_l, uint8_t in1_l, uint8_t in2_l);
    void updateMotorsFromInput(int16_t x, int16_t y);
    void run();


private:
    struct MotorState {
        L298N::Direction direction;
        uint32_t delayMs;
        uint8_t speed;
    };

    //Variables//
    struct {
        MotorState motorRight;
        MotorState motorLeft;
    } state;
    L298NX2 motors;

    //Methods//
    L298N::Direction speedToDirection(int8_t speed);
    bool inputToSpeed(int16_t x, int16_t y, int8_t& motorRight, int8_t& motorLeft);
    void inputTranslate(int16_t x, int16_t y);
};

#endif // __MOTORS_CONTROLLER_H