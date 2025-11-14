#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include <string>
#include <map>
#include "RemoteController.h"


enum class SimulatorState {
	kIdle = 0,
	kMoveForward,
	kMoveBackward,
	kForwardArcTurnRight,
	kBackwardArcTurnRight,
	kForwardArcTurnLeft,
	kBackwardArcTurnLeft,
	kPointTurnRight,
	kPointTurnLeft
};


class Simulator {
public:
  Simulator(unsigned int stateChangeMs);
  struct RemoteControllerData getData();
  void init();

private:
  //Constants//
  static const std::string kTag;
  const unsigned int kStateChangeMs;
  
  //Variables//
  unsigned int lastStateUpdateMs;
  std::map<SimulatorState, SimulatorState> simulatorTransition;
  std::map<SimulatorState, struct RemoteControllerData> simulatorData;
  SimulatorState currentState;
};

#endif // __SIMULATOR_H