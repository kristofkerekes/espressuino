#include "Button.h"
#include <Arduino.h>

Button::Button (int pin):
  pin (pin)
{
}

void  Button::Init ()
{
  pinMode (pin, INPUT);
}

bool  Button::GetState () const
{
  previousState = digitalRead (pin);
  return previousState;
}

bool  Button::GetPreviousState () const
{
  return previousState;
}
