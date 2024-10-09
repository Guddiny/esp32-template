#include "messaging.h"

void init_messaging() 
{
  buttonQueue = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(ButtonEvent_t));
}