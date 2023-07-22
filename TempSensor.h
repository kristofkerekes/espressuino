#include <Adafruit_MAX31865.h>
#include <deque>

#ifndef TEMP_SENSOR
#define TEMP_SENSOR

class TempSensor {
private:
  Adafruit_MAX31865 sensor;
  const max31865_numwires_t PT100_TYPE = MAX31865_2WIRE;
  double tempOffset = 0.0;
  double lastTemperature = 0.0;
  double lastAverageTemperature = 0.0;
  std::deque<double> lastTemperatures;

public:
  TempSensor (const max31865_numwires_t pt100_type, int8_t cs_pin);
  TempSensor (const max31865_numwires_t pt100_type, int8_t cs_pin, int8_t sdi_pin, int8_t sdo_pin, int8_t clk_pin);
  void Init ();

  double GetLastTemperature () const;
  double GetTemperature ();
  void   StoreTemperature ();
  double GetAverageTemperature ();
  double GetLastAverageTemperature () const;

  void SetTemperatureOffset (double newOffset);
};

#endif
