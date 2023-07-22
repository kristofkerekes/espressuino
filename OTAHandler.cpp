#include "OTAHandler.h"
#include <ArduinoOTA.h>
#if defined(ESP8266)
#include "FS.h"
#include <Updater.h>
#define U_SPIFFS U_FS
#else
#include "SPIFFS.h"
#include <Update.h>
#endif

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

void OTAHandler::Init (const std::function<void ()>& updateCompleted)
{
  this->updateCompleted = updateCompleted;
  
  ArduinoOTA.onStart ([] () {
    SPIFFS.end ();
  });

  ArduinoOTA.onEnd ([updateCompleted] () {
    updateCompleted ();
    ESP.restart ();
  });

  ArduinoOTA.onError ([] (ota_error_t error) {
    ESP.restart ();
  });

  ArduinoOTA.begin ();
}

void OTAHandler::Do ()
{
  ArduinoOTA.handle ();
}

void OTAHandler::UpdateFirmware (String jobName, size_t index, uint8_t* data, size_t len, bool final)
{
  if (jobName != "firmware" && jobName != "spiffs") {
    updateSuccess = false;
    return;
  }
  
  if (!index) {
    Serial.printf ("Start Firmware Update: %s\n", jobName.c_str ());
#if defined(ESP8266)
    Update.runAsync (true);
#endif
    if (jobName == "firmware") {
      if (!Update.begin ((ESP.getFreeSketchSpace () - 0x1000) & 0xFFFFF000), U_FLASH) {
        Update.printError (Serial);
      }
    } else if (jobName == "spiffs") {
#if defined(ESP8266)
      if (!Update.begin ((size_t) &_SPIFFS_end - (size_t) &_SPIFFS_start, U_SPIFFS)) {
#else
      if (!Update.begin (UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
#endif
        Update.printError (Serial);
      }
    }
  }

  if (!Update.hasError ()) {
    if (Update.write (data, len) != len) {
      Update.printError (Serial);
    }
  }

  if (final) {
    updateCompleted ();
    if (Update.end (true)) {
      Serial.printf ("Update Success: %uB\n", index + len);
    } else {
      Update.printError (Serial);
    }
  }

  updateSuccess = !Update.hasError ();
}

bool OTAHandler::IsUpdateSuccessful () const
{
  return updateSuccess;
}
