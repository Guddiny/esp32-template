#ifndef MESSAGING_H
#define MESSAGING_H

#include <Arduino.h>
#include "button.h"

#define BUTTON_QUEUE_SIZE 4

extern QueueHandle_t buttonQueue;

void init_messaging();

#endif