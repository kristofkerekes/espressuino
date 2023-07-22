#include <Arduino.h>
#include <functional>

#ifndef OTA_HANDLER
#define OTA_HANDLER

class OTAHandler {
private:
  bool updateSuccess = false;
  std::function<void ()> updateCompleted;
  
public:
  void Init (const std::function<void ()>& updateCompleted);
  void Do ();

  void UpdateFirmware (String jobName, size_t index, uint8_t* data, size_t len, bool final);
  bool IsUpdateSuccessful () const;
};

#endif
