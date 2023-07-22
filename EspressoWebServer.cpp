#include "EspressoWebServer.h"
#include <ArduinoJson.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include "FS.h"
#else
#include <WiFi.h>
#include "SPIFFS.h"
#endif

EspressoWebServer::EspressoWebServer (const std::function<String ()>& versionGetter,
                                      const std::function<String ()>& machineNameGetter,
                                      const std::function<void (const String&)>& machineNameSetter,
                                      const std::function<void ()>& parameterSaver,
                                      const std::function<double ()>& boilerTemperatureGetter,
                                      const std::function<double ()>& boilerTargetTemperatureGetter, 
                                      const std::function<void (double)>& boilerTargetTemperatureSetter,
                                      const std::function<void (double&, double&, double&, double&)>& pidParameterGetter,
                                      const std::function<void (double, double, double, double)>& pidParameterSetter,
                                      const std::function<void (const String&, const String&, bool, uint8_t)>& deviceParameterSetter,
                                      const std::function<void (String&, String&, bool&, uint8_t&)>& deviceParameterGetter,
                                      const std::function<void (String, size_t, uint8_t*, size_t, bool)>& firmwareUpdater,
                                      const std::function<bool ()>& updateSuccessfulGetter,
                                      const std::function<String ()>& autoTuningStatusGetter,
                                      const std::function<void (bool)>& autoTuningStatusSetter):
  server (80),
  versionGetter (versionGetter),
  machineNameGetter (machineNameGetter),
  machineNameSetter (machineNameSetter),
  parameterSaver (parameterSaver),
  boilerTemperatureGetter (boilerTemperatureGetter),
  boilerTargetTemperatureGetter (boilerTargetTemperatureGetter),
  boilerTargetTemperatureSetter (boilerTargetTemperatureSetter),
  pidParameterGetter (pidParameterGetter),
  pidParameterSetter (pidParameterSetter),
  deviceParameterSetter (deviceParameterSetter),
  deviceParameterGetter (deviceParameterGetter),
  firmwareUpdater (firmwareUpdater),
  updateSuccessfulGetter (updateSuccessfulGetter),
  autoTuningStatusGetter (autoTuningStatusGetter),
  autoTuningStatusSetter (autoTuningStatusSetter)
{
}

void EspressoWebServer::Init ()
{
  server.serveStatic ("/", SPIFFS, "/").setDefaultFile ("index.html");

  server.on ("/version", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    request->send_P (200, "text/plain", versionGetter ().c_str ());
  });

  server.on ("/machine_name", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    request->send_P (200, "text/plain", machineNameGetter ().c_str ());
  }); 
  
  server.on ("/restart_device", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    request->send (200);
    ESP.restart ();
  });
  
  server.on ("/save_parameters", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    parameterSaver ();
    request->send (200, "text/plain", "Parameters saved successfully");
  });
  
  server.on ("/boiler_temperature", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    const double boilerTemperature = boilerTemperatureGetter ();
    request->send_P (200, "text/plain", String (boilerTemperature).c_str ());
  });

  server.on ("/boiler_target_temperature", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    const double boilerTemperature = boilerTargetTemperatureGetter ();
    request->send_P (200, "text/plain", String (boilerTemperature).c_str ());
  });

  server.on ("/boiler_target_temperature", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      boilerTargetTemperatureSetter (message.toFloat ());
      request->send (200, "text/plain", String (message.toFloat ()).c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  server.on ("/proportional_gain", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    request->send_P (200, "text/plain", String (kpValue).c_str ());
  });

  server.on ("/proportional_gain", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      kpValue = message.toFloat ();
      
      pidParameterSetter (kpValue, kiValue, kdValue, tempOffset);
      request->send (200, "text/plain", String (message.toFloat ()).c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  server.on ("/integral_gain", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    request->send_P (200, "text/plain", String (kiValue).c_str ());
  });

  server.on ("/integral_gain", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      kiValue = message.toFloat ();
      
      pidParameterSetter (kpValue, kiValue, kdValue, tempOffset);
      request->send (200, "text/plain", String (message.toFloat ()).c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  server.on ("/derivative_gain", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    request->send_P (200, "text/plain", String (kdValue).c_str ());
  });

  server.on ("/derivative_gain", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      kdValue = message.toFloat ();
      
      pidParameterSetter (kpValue, kiValue, kdValue, tempOffset);
      request->send (200, "text/plain", String (message.toFloat ()).c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  
  server.on ("/temperature_offset", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    request->send_P (200, "text/plain", String (tempOffset).c_str ());
  });

  server.on ("/temperature_offset", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    double kpValue, kiValue, kdValue, tempOffset;
    pidParameterGetter (kpValue, kiValue, kdValue, tempOffset);
    
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      tempOffset = message.toFloat ();
      
      pidParameterSetter (kpValue, kiValue, kdValue, tempOffset);
      request->send (200, "text/plain", String (message.toFloat ()).c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  server.on ("/pid_autotune", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    String autoTuningStatus = autoTuningStatusGetter ();
    request->send_P (200, "text/plain", autoTuningStatus.c_str ());
  });

  server.on ("/pid_autotune", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    if (request->hasArg ("value")) {
      String message = request->arg ("value");
      bool isAutoTuning = message == "true";
      
      autoTuningStatusSetter (isAutoTuning);
      request->send (200, "text/plain", message.c_str ());
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });
  
  server.on ("/device_parameters", HTTP_GET, [this] (AsyncWebServerRequest* request) {
    String wifiSSID, wifiPassword;
    bool wifiMode;
    uint8_t stationAddr;
    deviceParameterGetter (wifiSSID, wifiPassword, wifiMode, stationAddr);

    DynamicJsonDocument responseJson (512);
    responseJson["wifiSSID"] = wifiSSID;
    responseJson["wifiPassword"] = wifiPassword;
    responseJson["wifiMode"] = wifiMode;
    responseJson["stationAddr"] = stationAddr;

    String response;
    serializeJson (responseJson, response);
            
    request->send_P (200, "application/json", response.c_str ());
  });

  server.on ("/device_parameters", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    if (request->hasArg ("wifiSSID") && request->hasArg ("wifiPassword") && request->hasArg ("wifiMode") && request->hasArg ("stationAddr") && request->hasArg ("machineName")) {
      String wifiSSID = request->arg ("wifiSSID");
      String wifiPassword = request->arg ("wifiPassword");
      bool wifiMode = request->arg ("wifiMode") == "true";
      uint8_t stationAddr = request->arg ("stationAddr").toInt ();

      String machineName = request->arg ("machineName");
      
      request->send (200, "text/plain", "Device Parameters set successfully");
      machineNameSetter (machineName);
      deviceParameterSetter (wifiSSID, wifiPassword, wifiMode, stationAddr);    
    } else {
      request->send (400, "text/plain", "Request not accepted. Wrong format in parameter message");
    }
  });

  server.on ("/update_firmware", HTTP_POST, [this] (AsyncWebServerRequest* request) {
    bool updateSuccess = updateSuccessfulGetter ();

    if (updateSuccess) {
      AsyncWebServerResponse* response = request->beginResponse (200, "text/plain", "Firmware successfully uploaded");
      response->addHeader ("Connection", "close");
      request->send (response);
    } else {
      AsyncWebServerResponse* response = request->beginResponse (400, "text/plain", "Failed to upload Firmware");
      response->addHeader ("Connection", "close");
      request->send (response);
    }
  },
  [this] (AsyncWebServerRequest* request, String jobName, size_t index, uint8_t* data, size_t len, bool final) {
    firmwareUpdater (jobName, index, data, len, final);
  });
  
  server.begin ();
}
