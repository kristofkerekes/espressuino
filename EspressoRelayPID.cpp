#include "EspressoRelayPID.h"
#include <Arduino.h>

namespace {
unsigned long lastPIDUpdate = 0;

const double AUTOTUNE_STEP = EspressoRelayPID::PID_INTERVAL;
const double AUTOTUNE_NOISE = 0.20;
  
}

EspressoRelayPID::EspressoRelayPID (double initialTarget, double kp, double ki, double kd, const std::function<void ()>& autotuningSuccessful):
  targetValue (initialTarget),
  pid (&inputValue, &outputValue, &targetValue, kp, ki, kd, P_ON_M, DIRECT),
  autoTuner (&inputValue, &outputValue, &targetValue),
  autotuningSuccessful (autotuningSuccessful)
{ 
}

void EspressoRelayPID::Init ()
{
  windowStartTime = millis ();
  pid.SetOutputLimits (0, WINDOW_SIZE);
  pid.SetMode (MANUAL);
  pid.SetSampleTime (PID_INTERVAL);

  autoTuner.SetOutputLimits (0, WINDOW_SIZE);
  autoTuner.SetSampleTime (PID_INTERVAL);
  autoTuner.SetNoiseBand (AUTOTUNE_NOISE);
}

double EspressoRelayPID::GetTargetValue () const 
{
  return targetValue;
}

void   EspressoRelayPID::SetTargetValue (double newTarget) 
{
  targetValue = newTarget;
}

void   EspressoRelayPID::SetPIDParameters (double kp, double ki, double kd)
{
  pid.SetTunings (kp, ki, kd, P_ON_M);
}

void   EspressoRelayPID::GetPIDParameters (double& kp, double& ki, double& kd)
{
  kp = pid.GetKp ();
  ki = pid.GetKi ();
  kd = pid.GetKd ();
}

bool EspressoRelayPID::IsAutoTuning () const
{
  return isAutoTuning;
}

void EspressoRelayPID::SetAutoTuning (bool state)
{
  isAutoTuning = state;

  if (IsAutoTuning ()) {
    autoTuner.Init ();
  } else {
    autoTuner.Cancel ();
  }
}

String EspressoRelayPID::GetAutoTuningResult () const
{
  if (autoTuner.GetState () == PIDAutoTuneRelay::FINISHED) {
    return "FINISHED\n" + GetDBInfo ();
  } else if (autoTuner.GetState () == PIDAutoTuneRelay::OFF) {
    return "OFF";    
  } else {
    return "ON";
  }
}

bool  EspressoRelayPID::ComputeOutput (double input)
{
  unsigned long now = millis ();
  inputValue = input;

  if (!IsAutoTuning ()) {
    if (inputValue - GetTargetValue () < -4.0) { // Under Target
      pid.SetMode (MANUAL);
      outputValue = WINDOW_SIZE;
    } else if (inputValue - GetTargetValue () > 1.0) { // Over Target
      pid.SetMode (MANUAL);
      outputValue = 0;
    } else {
      pid.SetMode (AUTOMATIC);
      pid.Compute ();
    }
  } else {
    isAutoTuning = autoTuner.Compute ();
    if (!IsAutoTuning () && autoTuner.GetState () == PIDAutoTuneRelay::FINISHED) {
      SetPIDParameters (autoTuner.GetKp (), autoTuner.GetKi (), autoTuner.GetKd ());
      autotuningSuccessful ();     
    }
  }

  if (now - windowStartTime > WINDOW_SIZE) {
    windowStartTime += WINDOW_SIZE;
  }
  
  if (outputValue < now - windowStartTime) {
    return false;
  } else {
    return true;
  }
}
