#include <Arduino.h>
#include <PID_v1.h>
#include <PID_AutoTune_Relay.h>
#include <functional>

#ifndef ESPRESSO_RELAY_PID
#define ESPRESSO_RELAY_PID

class EspressoRelayPID {
  private:
    PID pid;
    
    PIDAutoTuneRelay autoTuner;
    bool isAutoTuning = false;

    double targetValue;
    double inputValue;
    double outputValue;

    const int WINDOW_SIZE = 1000;
    unsigned long windowStartTime;

    std::function<void ()> autotuningSuccessful;

  public:
    static const unsigned long PID_INTERVAL = 200;
  
    EspressoRelayPID (double initialTarget, double kp, double ki, double kd, const std::function<void ()>& autotuningSuccessful);

    void Init ();

    double GetTargetValue () const;  
    void   SetTargetValue (double newTarget);
    
    void SetPIDParameters (double kp, double ki, double kd);
    void GetPIDParameters (double& kp, double& ki, double& kd);
    
    bool IsAutoTuning () const;
    void SetAutoTuning (bool state);
    String GetAutoTuningResult () const;
    
    bool ComputeOutput (double input);
};

#endif
