#include "RelayController.h"
#include <Arduino.h>

RelayController::RelayController (int relayPin):
  RelayPin (relayPin)
{
}

void RelayController::Init ()
{
  pinMode (RelayPin, OUTPUT);
  digitalWrite (RelayPin, HIGH);
}

void RelayController::Set (bool enabled)
{
  digitalWrite (RelayPin, enabled ? HIGH : LOW);
}
