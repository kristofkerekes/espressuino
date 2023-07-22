#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#ifndef WIFI_CONNECTION
#define WIFI_CONNECTION

class WiFiConnection {
private:
  bool needsReconnect;

  String ssid;
  String password;
  bool apMode;
  uint8_t stationAddr;
  
public:
  WiFiConnection (const String& ssid, const String& pw, bool apMode, uint8_t stationAddr);

  void InitStationMode ();
  void InitAccessPointMode ();

  void Do ();
  void Init ();
  bool IsConnected () const;
  void Reconnect ();

  void SetParameters (const String& newSSID, const String& newPw, bool newMode, uint8_t newStationAddr);
};

#endif
