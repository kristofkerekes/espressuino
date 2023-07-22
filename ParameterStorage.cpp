#include "ParameterStorage.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#if defined(ESP8266)
#include "FS.h"
#else
#include "SPIFFS.h"
#endif

namespace {
const char* CONFIG_FILE_NAME = "/config.json";

const char* TARGET_BOILER_TEMP_NAME = "targetBoilerTemp";

const char* KP_VALUE_NAME = "kpValue";
const char* KI_VALUE_NAME = "kiValue";
const char* KD_VALUE_NAME = "kdValue";

const char* TEMP_OFFSET_NAME = "tempOffset";

const char* WIFI_SSID_NAME = "wifiSSID";
const char* WIFI_PASSWORD_NAME = "wifiPassword";
const char* WIFI_AP_MODE_NAME = "wifiAPMode";
const char* WIFI_STATION_ADDR_NAME = "stationAddr";

const char* MACHINE_NAME_NAME = "machineName";
}

ParameterStorage::ParameterStorage ()
{

}

void ParameterStorage::Init ()
{
  LoadParameters ();
}

double ParameterStorage::GetTargetBoilerTemp () const
{
  return targetBoilerTemp;
}

void   ParameterStorage::SetTargetBoilerTemp (double newTargetBoilerTemp)
{
  targetBoilerTemp = newTargetBoilerTemp;
}

double ParameterStorage::GetKpValue () const
{
  return kpValue;
}

double ParameterStorage::GetKiValue () const
{
  return kiValue;
}

double ParameterStorage::GetKdValue () const
{
  return kdValue;
}

double ParameterStorage::GetTempOffset () const
{
  return tempOffset;
}

String ParameterStorage::GetWiFiSSID () const
{
  return wifiSSID;
}

String ParameterStorage::GetWiFiPassword () const
{
  return wifiPassword;
}

bool ParameterStorage::GetWiFiAPMode   () const
{
  return wifiAPMode;
}

uint8_t ParameterStorage::GetStationAddr () const
{
  return stationAddr;
}

void ParameterStorage::SetKpValue (double newKp)
{
  kpValue = newKp;
}

void ParameterStorage::SetKiValue (double newKi)
{
  kiValue = newKi;
}

void ParameterStorage::SetKdValue (double newKd)
{
  kdValue = newKd;
}

void ParameterStorage::SetTempOffset (double newOffset)
{
  tempOffset = newOffset;
}

void ParameterStorage::SetWiFiParameters (const String& newSSID, const String& newPw, bool newMode, uint8_t newStationAddr)
{
  wifiSSID = newSSID;
  wifiPassword = newPw;
  wifiAPMode = newMode;
  stationAddr = newStationAddr;
}

String ParameterStorage::GetMachineName () const
{
  return machineName;
}

void ParameterStorage::SetMachineName (const String& newName)
{
  machineName = newName;
}

void  ParameterStorage::SaveParameters ()
{
  File configFile = SPIFFS.open (CONFIG_FILE_NAME, "w+");
  if (!configFile)
    return;

  DynamicJsonDocument configJson (512);
  DeserializationError error = deserializeJson (configJson, configFile);

  configJson[TARGET_BOILER_TEMP_NAME] = targetBoilerTemp;
  
  configJson[KP_VALUE_NAME] = kpValue;
  configJson[KI_VALUE_NAME] = kiValue;
  configJson[KD_VALUE_NAME] = kdValue;
  configJson[TEMP_OFFSET_NAME] = tempOffset;

  configJson[WIFI_SSID_NAME] = wifiSSID;
  configJson[WIFI_PASSWORD_NAME] = wifiPassword;
  configJson[WIFI_AP_MODE_NAME] = wifiAPMode;
  configJson[WIFI_STATION_ADDR_NAME] = stationAddr;

  configJson[MACHINE_NAME_NAME] = machineName;

  serializeJson (configJson, configFile);
  configFile.close ();
}

void  ParameterStorage::LoadParameters ()
{
  File configFile = SPIFFS.open (CONFIG_FILE_NAME, "r");
  if (!configFile)
    return;
  
  DynamicJsonDocument configJson (512);
  DeserializationError error = deserializeJson (configJson, configFile);
  if (error)
    return;

  if (configJson.containsKey (TARGET_BOILER_TEMP_NAME))
    targetBoilerTemp = configJson[TARGET_BOILER_TEMP_NAME];

    
  if (configJson.containsKey (KP_VALUE_NAME))
    kpValue = configJson[KP_VALUE_NAME];
    
  if (configJson.containsKey (KI_VALUE_NAME))
    kiValue = configJson[KI_VALUE_NAME];
        
  if (configJson.containsKey (KD_VALUE_NAME))
    kdValue = configJson[KD_VALUE_NAME];
   
  if (configJson.containsKey (TEMP_OFFSET_NAME))
    tempOffset = configJson[TEMP_OFFSET_NAME];

         
  if (configJson.containsKey (WIFI_SSID_NAME))
    wifiSSID = configJson[WIFI_SSID_NAME].as<String> ();
         
  if (configJson.containsKey (WIFI_PASSWORD_NAME))
    wifiPassword = configJson[WIFI_PASSWORD_NAME].as<String> ();
         
  if (configJson.containsKey (WIFI_AP_MODE_NAME))
    wifiAPMode = configJson[WIFI_AP_MODE_NAME];
         
  if (configJson.containsKey (WIFI_STATION_ADDR_NAME))
    stationAddr = configJson[WIFI_STATION_ADDR_NAME];
    

  if (configJson.containsKey (MACHINE_NAME_NAME))
    machineName = configJson[MACHINE_NAME_NAME].as<String> ();
            
  configFile.close ();
}

void ParameterStorage::ResetToDefaults ()
{
  targetBoilerTemp = 90.0;
  
  kpValue = 40.00;
  kiValue = 0.50;
  kdValue = 170.00;
  tempOffset = 0.00;

  wifiSSID = "Espresso-PID-AP";
  wifiPassword = "Abcd1234";
  wifiAPMode = true;
  stationAddr = 234;

  machineName = "Espresso PID";
  
  SaveParameters ();
}
