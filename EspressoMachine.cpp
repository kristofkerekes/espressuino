#include "EspressoMachine.h"
#include "OTAHandler.h"

namespace {

constexpr int8_t BOILER_SSR_PIN = 4;

#if defined(ESP8266)
constexpr int8_t TEMP_SENSOR_CLK_PIN = 13;
constexpr int8_t TEMP_SENSOR_SDO_PIN = 12;
constexpr int8_t TEMP_SENSOR_SDI_PIN = 14;
constexpr int8_t TEMP_SENSOR_CS_PIN  = 5;
#else
constexpr int8_t TEMP_SENSOR_CLK_PIN = 27;
constexpr int8_t TEMP_SENSOR_SDO_PIN = 26;
constexpr int8_t TEMP_SENSOR_SDI_PIN = 25;
constexpr int8_t TEMP_SENSOR_CS_PIN  = 33;
#endif

}

EspressoMachine::EspressoMachine (OTAHandler& otaHandler):
  paramStorage (),
  tempSensor (PT100_TYPE, TEMP_SENSOR_CS_PIN, TEMP_SENSOR_SDI_PIN, TEMP_SENSOR_SDO_PIN, TEMP_SENSOR_CLK_PIN),
  wifiConnection (paramStorage.GetWiFiSSID (), paramStorage.GetWiFiPassword (), paramStorage.GetWiFiAPMode (), paramStorage.GetStationAddr ()),
  webServer ([this] () -> String       { return String (MAJOR_VERSION) + "." + String (MINOR_VERSION); },
             [this] () -> String       { return paramStorage.GetMachineName (); },
             [this] (const String& newName) -> void { return paramStorage.SetMachineName (newName); },
             [this] () -> void         { paramStorage.SaveParameters (); },
             [this] () -> double       { return tempSensor.GetAverageTemperature (); },
             [this] () -> double       { return boilerPID.GetTargetValue (); },
             [this] (double newTarget) { paramStorage.SetTargetBoilerTemp (newTarget); boilerPID.SetTargetValue (paramStorage.GetTargetBoilerTemp ()); },
             [this] (double& kpValue, double& kiValue, double& kdValue, double& tempOffset) { kpValue = paramStorage.GetKpValue (); kiValue = paramStorage.GetKiValue (); kdValue = paramStorage.GetKdValue (); tempOffset = paramStorage.GetTempOffset (); },
             [this] (double newKp, double newKi, double newKd, double newOffset) { paramStorage.SetKpValue (newKp); paramStorage.SetKiValue (newKi); paramStorage.SetKdValue (newKd); paramStorage.SetTempOffset (newOffset);
                                                                                   tempSensor.SetTemperatureOffset (paramStorage.GetTempOffset ());
                                                                                   boilerPID.SetPIDParameters (paramStorage.GetKpValue (), paramStorage.GetKiValue (), paramStorage.GetKdValue ());},
             [this] (const String& newWifiSSID, const String& newWifiPw, bool newMode, uint8_t newStationAddr) { paramStorage.SetWiFiParameters (newWifiSSID, newWifiPw, newMode, newStationAddr);
                                                                                         wifiConnection.SetParameters (paramStorage.GetWiFiSSID (), paramStorage.GetWiFiPassword (), paramStorage.GetWiFiAPMode (), paramStorage.GetStationAddr ()); },
             [this] (String& wifiSSID, String& wifiPw, bool& wifiMode, uint8_t& stationAddr) { wifiSSID = paramStorage.GetWiFiSSID (); wifiPw = paramStorage.GetWiFiPassword (); wifiMode = paramStorage.GetWiFiAPMode (); stationAddr = paramStorage.GetStationAddr (); },
             [&otaHandler] (String filename, size_t index, uint8_t* data, size_t len, bool final) { otaHandler.UpdateFirmware (filename, index, data, len, final); },
             [&otaHandler] () -> bool { return otaHandler.IsUpdateSuccessful (); },
             [this] () -> String   { return boilerPID.GetAutoTuningResult (); },
             [this] (bool state) { boilerPID.SetAutoTuning (state); }),
  boilerPID (paramStorage.GetTargetBoilerTemp (), paramStorage.GetKpValue (), paramStorage.GetKiValue (), paramStorage.GetKdValue (),
             [this] () -> void { double kpValue, kiValue, kdValue; boilerPID.GetPIDParameters (kpValue, kiValue, kdValue); 
                                 paramStorage.SetKpValue (kpValue); paramStorage.SetKiValue (kiValue); paramStorage.SetKdValue (kdValue); paramStorage.SaveParameters (); }),
  boilerSSR (BOILER_SSR_PIN)
{ 
}


void EspressoMachine::Init ()
{
  Serial.println ("Starting Parameter Storage Initialization");
  paramStorage.Init ();
  Serial.println ("Parameter Storage Initialized");
  
  Serial.println ("Filling Parameters from Storage");
  RefillParameters ();
  Serial.println ("Parameters filled from Storage");
  
  Serial.println ("Starting Temp Sensor Initialization");
  tempSensor.Init ();
  Serial.println ("Temp Sensor Initialized");

  Serial.println ("Starting WiFi Initialization");
  wifiConnection.Init ();
  Serial.println ("WiFi Initialized");

  Serial.println ("Starting Web Server Initialization");
  webServer.Init ();
  Serial.println ("Web Server Initialized");

  Serial.println ("Starting Boiler PID Initialization");
  boilerPID.Init ();
  Serial.println ("Boiler PID Initialized");

  Serial.println ("Starting Boiler SSR Initialization");
  boilerSSR.Init ();
  Serial.println ("Boiler SSR Initialized");
}


void  EspressoMachine::RefillParameters ()
{
  wifiConnection.SetParameters (paramStorage.GetWiFiSSID (), paramStorage.GetWiFiPassword (), paramStorage.GetWiFiAPMode (), paramStorage.GetStationAddr ());

  tempSensor.SetTemperatureOffset (paramStorage.GetTempOffset ());  
  
  boilerPID.SetTargetValue (paramStorage.GetTargetBoilerTemp ());
  boilerPID.SetPIDParameters (paramStorage.GetKpValue (), paramStorage.GetKiValue (), paramStorage.GetKdValue ());
}

void EspressoMachine::Compute ()
{
  tempSensor.StoreTemperature ();
  
  bool boilerRelayState = boilerPID.ComputeOutput (tempSensor.GetLastTemperature ());
  boilerSSR.Set (boilerRelayState);

  wifiConnection.Do ();
}

void EspressoMachine::ResetToDefaults ()
{
  paramStorage.ResetToDefaults ();
}

void EspressoMachine::UpdateCompleted ()
{
  paramStorage.SaveParameters ();
}
