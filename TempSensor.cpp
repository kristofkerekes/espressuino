#include "TempSensor.h"


TempSensor::TempSensor (const max31865_numwires_t pt100_type, int8_t cs_pin):
  sensor (Adafruit_MAX31865 (cs_pin)),
  PT100_TYPE (pt100_type)
{
}
TempSensor::TempSensor (const max31865_numwires_t pt100_type, int8_t cs_pin, int8_t sdi_pin, int8_t sdo_pin, int8_t clk_pin):
  sensor (Adafruit_MAX31865 (cs_pin, sdi_pin, sdo_pin, clk_pin)),
  PT100_TYPE (pt100_type)
{
}

void TempSensor::Init ()
{
  sensor.begin (PT100_TYPE);
}

double TempSensor::GetTemperature ()
{
  lastTemperature = sensor.temperature (100.0, 430.0) + tempOffset;
  return lastTemperature;
}

void TempSensor::StoreTemperature ()
{
  if (lastTemperatures.size () > 20) {
    lastTemperatures.pop_front ();
  }
  
  lastTemperatures.push_back (GetTemperature ()); 
}

double TempSensor::GetLastTemperature () const
{
  return lastTemperature;
}

double TempSensor::GetAverageTemperature ()
{
  lastAverageTemperature = std::accumulate (lastTemperatures.begin (), lastTemperatures.end (), 0.0) / lastTemperatures.size ();
  return lastAverageTemperature;
}


double TempSensor::GetLastAverageTemperature () const
{
  return lastAverageTemperature;
}

void TempSensor::SetTemperatureOffset (double newOffset)
{
  tempOffset = newOffset;
}
