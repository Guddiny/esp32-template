#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

typedef enum
{
  RELEASED,
  PRESSED,
  ON_HOLD
} ButtonState_t;

typedef struct
{
  gpio_num_t pin;
  ButtonState_t state;
} ButtonEvent_t;

#endif