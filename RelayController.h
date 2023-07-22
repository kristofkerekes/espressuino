#ifndef RELAY_CONTROLLER
#define RELAY_CONTROLLER

class RelayController {
private:
  const int RelayPin;
  
public:
  RelayController (int relayPin);

  void Init ();
  void Set (bool enabled);
};

#endif
