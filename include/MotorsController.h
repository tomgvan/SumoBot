#pragma once

#include "Gpios.h"
#include "HBridgeMotor.h"


struct MotorsControllerConfig {
    HBridgeMotorPins rightPins;
    HBridgeMotorPins leftPins;
    unsigned int minMotorSpeed;
    unsigned int maxMotorSpeed;
};


class MotorsController {
public:
    //Constants//
    static constexpr MotorsControllerConfig kDefaultConfig {
        .rightPins = {
            .en   = gpios::kMotorRightEn, 
            .in1  = gpios::kMotorRightIn1, 
            .in2  = gpios::kMotorRightIn2
        },
        .leftPins = {
            .en   = gpios::kMotorLeftEn, 
            .in1  = gpios::kMotorLeftIn1, 
            .in2  = gpios::kMotorLeftIn2
        },
        .minMotorSpeed = 0,
        .maxMotorSpeed = 255
    };

    //Methods//
    explicit MotorsController(const MotorsControllerConfig& motorsConfig = kDefaultConfig);
    void init();
    void updateSpeed(unsigned int speedR, unsigned int speedL);
    void updateDirection(HBridgeMotor::Direction directionR, HBridgeMotor::Direction directionL);
    void stop();

private:
    //Constants//
    static constexpr const char* kTag {"Motors Controller"};

    //Variables//
    HBridgeMotor motorRight;
    HBridgeMotor motorLeft;
};