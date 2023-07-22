#include "ParameterStorage.h"
#include "TempSensor.h"
#include "WiFiConnection.h"
#include "EspressoWebServer.h"
#include "EspressoRelayPID.h"
#include "RelayController.h"

#ifndef ESPRESSO_MACHINE
#define ESPRESSO_MACHINE

class OTAHandler;

class EspressoMachine {
private:
  const int8_t MAJOR_VERSION = 1;
  const int8_t MINOR_VERSION = 6;
  const max31865_numwires_t PT100_TYPE = MAX31865_2WIRE;

  ParameterStorage paramStorage;
  
  TempSensor tempSensor;
  
  WiFiConnection wifiConnection;
  EspressoWebServer webServer;

  EspressoRelayPID boilerPID;
  RelayController boilerSSR;

public:
  EspressoMachine (OTAHandler& otaHandler);
  
  void  Init ();
  void  RefillParameters ();
  void  Compute ();

  void ResetToDefaults ();
  void UpdateCompleted ();
};

#endif
