#include <ESPAsyncWebServer.h>

#ifndef ESPRESSO_WEB_SERVER
#define ESPRESSO_WEB_SERVER

class EspressoWebServer {
  private:
    AsyncWebServer server;

    std::function<String ()> versionGetter;

   std::function<String ()> machineNameGetter;
   std::function<void (const String&)> machineNameSetter;
                       
    std::function<void ()> parameterSaver;
    
    std::function<double ()> boilerTemperatureGetter;
    std::function<double ()> boilerTargetTemperatureGetter;
    std::function<void (double)> boilerTargetTemperatureSetter;
    
    std::function<void (double&, double&, double&, double&)> pidParameterGetter;
    std::function<void (double, double, double, double)> pidParameterSetter;
    
    std::function<void (const String&, const String&, bool, uint8_t)> deviceParameterSetter;
    std::function<void (String&, String&, bool&, uint8_t&)> deviceParameterGetter;
    
    std::function<void (String, size_t, uint8_t*, size_t, bool)> firmwareUpdater;
    std::function<bool ()> updateSuccessfulGetter;

    std::function<String ()> autoTuningStatusGetter;
    std::function<void (bool)> autoTuningStatusSetter;

  public:
    EspressoWebServer (const std::function<String ()>& versionGetter,
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
                       const std::function<void (bool)>& autoTuningStatusSetter);

    void Init ();
};

#endif
