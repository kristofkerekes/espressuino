#include "FlowController.h"
#include <Arduino.h>

FlowController::FlowController (RelayController& pumpSSR):
  pumpSSR (pumpSSR) 
{
}

void FlowController::RampPump (const std::vector<int>& sequence, unsigned long targetTime)
{
  unsigned long sequencePartTime = targetTime / sequence.size ();
  unsigned long sequenceStart = millis ();

  for (int i = 0; i < sequence.size ();) {
    for (int j = 0; j < 10; ++j) {
      pumpSSR.Set (pumpSequence[sequence[i]][j]);
      delay (20);
    }
    
    if (sequenceStart + sequencePartTime < millis ()) {
      ++i;
      sequenceStart = millis ();
    }   
  }     
}

void FlowController::HoldPump (int targetFlow, unsigned long targetTime)
{
  RampPump ({targetFlow}, targetTime);
}

void FlowController::Preinfuse ()
{
  RampPump ({1, 2, 3, 4, 5, 7, 9}, piRampUpTime);
  HoldPump (0, piHoldTime);
}

void FlowController::RampUpAndHold ()
{
  RampPump ({1, 2, 3, 4, 5, 6, 7, 8, 9}, ruhRampUpTime);
  HoldPump (10, ruhHoldTime);
}

void FlowController::Decline ()
{
  RampPump ({9, 8, 7, 6, 5, 4, 3}, decRampDownTime);
  HoldPump (3, 60000);
}
