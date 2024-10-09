#include <Arduino.h>
#include "task.h"
#include "messaging.h"

QueueHandle_t buttonQueue;

void setup()
{
  ESP_LOGI("*", "STARTED");

  // Enabled ISR service
  gpio_install_isr_service(0);

  // Init tasks
  init_led_task();
  init_log_task();
  init_button_task();
  init_messaging();

  // Run tasks
  run_led_task();
  run_log_task();
  run_button_task();

  // Free up the Arduino loop task
  vTaskDelete(NULL);
}

void loop() {}