#include <Arduino.h>
#include "task.h"
#include "messaging.h"

#define LED GPIO_NUM_2

uint8_t led_state = 0;
ButtonEvent_t button_event;
TaskHandle_t xHandle;

static void led_task_handler(void *params);
static void led_blink(void *params);

void init_led_task()
{
  gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  ESP_LOGI("*", "LED INITIATED");
}

void run_led_task()
{
  xTaskCreatePinnedToCore(led_task_handler, "LED_TASK", 2024, NULL, tskIDLE_PRIORITY, NULL, 1);
  //xTaskCreatePinnedToCore(led_blink, "LED_BLINK_TASK", 1024, NULL, tskIDLE_PRIORITY, &xHandle, 1);
  //vTaskSuspend(&xHandle);
}

static void led_task_handler(void *params)
{
  ESP_LOGI("*", "LED TASK STARTED");

  for (;;)
  {
    xQueueReceive(buttonQueue, &button_event, portMAX_DELAY);

    if (button_event.state == PRESSED)
    {
      gpio_set_level(LED, HIGH);
    }
    else if (button_event.state == RELEASED)
    {
      gpio_set_level(LED, LOW);
    }
    else if (button_event.state == ON_HOLD)
    {
      //TODO Start/Stop Led Blink Task
      ESP_LOGI("*", "LED BLINK");
    }
  }
}

static void led_blink(void *params)
{
  for (;;)
  {
    led_state ^= 1;

    ESP_LOGI("*", "LED state %d.", led_state);

    gpio_set_level(LED, led_state);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}