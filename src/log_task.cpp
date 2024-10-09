#include <Arduino.h>
#include "task.h"

static void log_task_handler(void *params);

void init_log_task()
{
  ESP_LOGI("*", "LOG INITIATED");
}

void run_log_task()
{
  xTaskCreatePinnedToCore(log_task_handler, "LOG_TASK", 2024, NULL, tskIDLE_PRIORITY, NULL, 1);
}

static void log_task_handler(void *params)
{
  ESP_LOGI("*", "LOG TASK STARTED");

  for (;;)
  {
    ESP_LOGI("*", "LOG ------------ LOG");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}