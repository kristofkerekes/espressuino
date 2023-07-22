#include "EspressoMachine.h"
#include "OTAHandler.h"
#if defined(ESP8266)
#include "FS.h"
#include <DoubleResetDetector.h>
#else
#include "SPIFFS.h"
#endif

const int8_t RESET_TIMEOUT = 5;

OTAHandler otaHandler;
EspressoMachine machine (otaHandler);

#if defined(ESP8266)
DoubleResetDetector resetDetector (RESET_TIMEOUT, 0);
#endif

void setup () 
{
  Serial.begin (115200);

  Serial.println ("#############################################");
  Serial.println ("Begin Initialization");  

  if(!SPIFFS.begin ())
    Serial.println ("An Error has occurred while mounting SPIFFS");
  else
    Serial.println ("File System Initialized");
    
#if defined(ESP8266)
  if (resetDetector.detectDoubleReset ()) {
    Serial.println ("Reset Detected: Restoring default parameters");
    machine.ResetToDefaults ();
  }
#endif
  
  machine.Init ();
  Serial.println ("Finished Machine Initialization");
 
  otaHandler.Init ([&] () {
    Serial.println ("Running Update Post-Process");
    
    if(!SPIFFS.begin ())
      Serial.println ("An Error has occurred while mounting SPIFFS");
    else
      Serial.println ("File System Initialized");
    
    machine.UpdateCompleted ();

    SPIFFS.end ();
  });
  Serial.println ("Finished OTA Initialization");
}

void loop () 
{
  machine.Compute ();
  otaHandler.Do ();

#if defined(ESP8266)  
  resetDetector.loop ();
#endif
}
