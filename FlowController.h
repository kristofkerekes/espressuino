#include "RelayController.h"
#include <vector>

class FlowController {
private:
  RelayController& pumpSSR;

  const int pumpSequence[11][10] = 
  {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,1},
    {0,0,0,1,0,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,0},
    {1,0,1,0,1,0,1,0,1,0},
    {0,1,1,0,1,0,1,1,0,1},
    {1,1,1,0,1,1,0,1,1,0},
    {1,1,1,1,0,1,1,1,1,0},
    {1,1,1,1,0,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1}
  }; 

  unsigned long piRampUpTime = 6000;
  unsigned long piHoldTime = 5000;

  unsigned long ruhRampUpTime = 3000;
  unsigned long ruhHoldTime = 10000;

  unsigned long decRampDownTime = 5000;
  
public:
  FlowController (RelayController& pumpSSR);
  
  void RampPump (const std::vector<int>& sequence, unsigned long targetTime);
  void HoldPump (int targetFlow, unsigned long targetTime);
  
  void Preinfuse ();
  void RampUpAndHold ();
  void Decline ();
};
