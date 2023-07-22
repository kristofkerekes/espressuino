#ifndef PARAMETER_STORAGE
#define PARAMETER_STORAGE

#include <Arduino.h>

class ParameterStorage {
private:
  double targetBoilerTemp = 90.0;
  
  double kpValue = 40.00;
  double kiValue = 0.50;
  double kdValue = 170.00;
  double tempOffset = 0.00;

  String wifiSSID = "Espresso-PID-AP";
  String wifiPassword = "Abcd1234";
  bool   wifiAPMode = true;
  uint8_t stationAddr = 234;

  String machineName = "Espresso PID";

public:
  ParameterStorage ();

  void Init ();

  double GetTargetBoilerTemp () const;
  void   SetTargetBoilerTemp (double newTargetBoilerTemp);
  
  double GetKpValue () const;
  double GetKiValue () const;
  double GetKdValue () const;
  double GetTempOffset () const;

  String GetWiFiSSID     () const;
  String GetWiFiPassword () const;
  bool   GetWiFiAPMode   () const;
  uint8_t GetStationAddr () const;

  void SetKpValue (double newKp);
  void SetKiValue (double newKi);
  void SetKdValue (double newKd);
  void SetTempOffset (double newOffset);

  void SetWiFiParameters (const String& newSSID, const String& newPw, bool newMode, uint8_t newStationAddr);

  String GetMachineName () const;
  void   SetMachineName (const String& newName);

  void SaveParameters ();
  void LoadParameters ();

  void ResetToDefaults ();
};

#endif
