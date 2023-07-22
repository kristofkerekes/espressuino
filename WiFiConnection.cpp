#include "WiFiConnection.h"
#if !defined(ESP8266)
#include <esp_wifi.h>
#endif

namespace {

void TurnOffPowerSaving ()
{
#if defined(ESP8266)
  wifi_set_sleep_type (NONE_SLEEP_T);
#else
  esp_wifi_set_ps (WIFI_PS_NONE);
#endif
}

const char* DEFAULT_SSID = "Espresso-PID-AP";
const char* DEFAULT_PW = "Abcd1234";

const IPAddress apIpAddress (192, 168, 99, 1);
const IPAddress apMask (255, 255, 255, 0);
  
}

WiFiConnection::WiFiConnection (const String& ssid, const String& pw, bool apMode, uint8_t stationAddr):
  needsReconnect (false),
  ssid (ssid),
  password (pw),
  apMode (apMode),
  stationAddr (stationAddr)
{

}

void WiFiConnection::InitStationMode ()
{
  WiFi.mode (WIFI_STA);
  TurnOffPowerSaving ();

  //Serial.println ("Initializing Default Access Point...");
  //WiFi.softAP (DEFAULT_SSID, DEFAULT_PW);
  
  //WiFi.softAPConfig (apIpAddress, apIpAddress, apMask);
  //Serial.println ("Default Access Point Initialized...");
  
  WiFi.begin (ssid.c_str (), password.c_str ());
  while (WiFi.status () != WL_CONNECTED) {
    Serial.println ("WiFi Connecting...");
    delay (1000);
  }

  IPAddress gateway (WiFi.gatewayIP ());
  IPAddress mask (WiFi.subnetMask ());
  IPAddress ipAddress = gateway; ipAddress[3] = stationAddr;

  if (WiFi.localIP () != ipAddress) {
    WiFi.config (ipAddress, gateway, mask);
  }

  Serial.print ("Local Address: ");
  Serial.println (WiFi.localIP ());
}

void WiFiConnection::InitAccessPointMode ()
{
  WiFi.mode (WIFI_AP);
  TurnOffPowerSaving ();

  WiFi.softAP (ssid.c_str (), password.c_str ());
  delay (1000);

  IPAddress ipAddress (192, 168, 1, 1);
  IPAddress mask (255, 255, 255, 0);

  WiFi.softAPConfig (ipAddress, ipAddress, mask);

  Serial.print ("IP Address: ");
  Serial.println (WiFi.softAPIP ());
}

void WiFiConnection::Do ()
{
  if (needsReconnect)
    Reconnect ();

  needsReconnect &= WiFi.status () == WL_CONNECTED;
}

void WiFiConnection::Init ()
{
  if (!apMode) {
    InitStationMode ();
  } else {
    InitAccessPointMode ();
  }
}

bool WiFiConnection::IsConnected () const
{
  return WiFi.status () == WL_CONNECTED;
}

void WiFiConnection::Reconnect ()
{
  needsReconnect = false;
  Serial.println ("Reconnecting...");
  Init ();
}


void WiFiConnection::SetParameters (const String& newSSID, const String& newPw, bool newMode, uint8_t newStationAddr)
{
  needsReconnect = ssid != newSSID || password != newPw || apMode != newMode || stationAddr != newStationAddr;

  ssid = newSSID;
  password = newPw;
  apMode = newMode;
  stationAddr = newStationAddr;
}
