#include "Simulator.h"


// Static Constants Initialization //
const std::string Simulator::kTag {"Simulator"};


Simulator::Simulator(
  unsigned int stateChangeMs) : 
    kStateChangeMs(stateChangeMs), 
    simulatorTransition ({
      {SimulatorState::kIdle,                     SimulatorState::kMoveForward},
      {SimulatorState::kMoveForward,              SimulatorState::kMoveBackward},
      {SimulatorState::kMoveBackward,             SimulatorState::kForwardArcTurnRight},
      {SimulatorState::kForwardArcTurnRight,      SimulatorState::kBackwardArcTurnRight},
      {SimulatorState::kBackwardArcTurnRight,     SimulatorState::kForwardArcTurnLeft},
      {SimulatorState::kForwardArcTurnLeft,       SimulatorState::kBackwardArcTurnLeft},
      {SimulatorState::kBackwardArcTurnLeft,      SimulatorState::kPointTurnRight},
      {SimulatorState::kPointTurnRight,           SimulatorState::kPointTurnLeft},
      {SimulatorState::kPointTurnLeft,            SimulatorState::kMoveForward}
    }),
    simulatorData ({
      {SimulatorState::kIdle,                     {true,    0,      0,      0,    0}},    //Not moving
      {SimulatorState::kMoveForward,              {false,   0,      -512,   1023, 0}},    //Forward full throttle
      {SimulatorState::kMoveBackward,             {false,   0,      512,    0,    1023}}, //Backward full throttle
      {SimulatorState::kForwardArcTurnRight,      {false,   362,    -362,   256,  0}},    //Forawrd arc right full throttle(45deg)
      {SimulatorState::kBackwardArcTurnRight,     {false,   362,    362,    0,    0}},    //Backward arc right full throttle(45deg)
      {SimulatorState::kForwardArcTurnLeft,       {false,   -481,   -175,   128,  0}},    //Forward arc left partial throttle(20deg)
      {SimulatorState::kBackwardArcTurnLeft,      {false,   -481,   175,    0,    128}},  //Backward arc left partial throttle(20deg)
      {SimulatorState::kPointTurnRight,           {false,   256,    0,      0,    0}},    //Point turn right half throttle
      {SimulatorState::kPointTurnLeft,            {false,   0,      -256,   0,    256}},  //Point turn left half throttle
    }),
    currentState(SimulatorState::kIdle),
    lastStateUpdateMs(0) {

}


struct RemoteControllerData Simulator::getData() {
  if(currentState != SimulatorState::kIdle && 
        millis() - lastStateUpdateMs < kStateChangeMs)
    return simulatorData.at(currentState);
  // if(currentState != SimulatorState::kIdle && 
  //       millis() - lastStateUpdateMs < kStateChangeMs)
  //   return simulatorData.at(SimulatorState::kIdle);

  lastStateUpdateMs = millis();

  ESP_LOGV(kTag.c_str(), "Old state: %d", currentState);
  currentState = simulatorTransition.at(currentState);
  ESP_LOGV(kTag.c_str(), "New state: %d", currentState);

  return simulatorData.at(currentState);
}