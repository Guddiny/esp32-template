#include <Arduino.h>
#include "task.h"
#include "button_task.h"
#include "button.h"
#include "messaging.h"
#include "utils.h"

#define MAIN_BUTTON_PIN GPIO_NUM_21
#define MASK 0b1111000000111111

const uint8_t buttons_count = 1;

typedef struct
{
  gpio_num_t pin;
  bool inverted;
  uint16_t history;
  uint32_t down_time;
  uint32_t next_long_time;
} Button_t;

Button_t *buttons;

static void update_button(Button_t *btn);
static void button_handler(void *params);
static bool is_button_up(Button_t *btn);
static bool is_button_down(Button_t *btn);
static void send_event(gpio_num_t pin, ButtonState_t state);

void init_button_task()
{
  buttons = (Button_t *)calloc(buttons_count, sizeof(Button_t));
  // queue = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(ButtonEvent_t));

  // Super bad. It should be filed from config
  buttons[0].pin = MAIN_BUTTON_PIN;
  buttons[0].inverted = true;

  for (size_t i = 0; i < buttons_count; i++)
  {
    gpio_pad_select_gpio(buttons[i].pin);
    gpio_set_direction(buttons[i].pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(buttons[i].pin, GPIO_FLOATING);
    // gpio_isr_handler_add(buttons[i].pin, isrButtonPress, NULL);
    // gpio_set_intr_type(buttons[i].pin, GPIO_INTR_ANYEDGE);
    // gpio_intr_enable(buttons[i].pin);
  }

  ESP_LOGI("*", "BUTTONS INITIATED");
}

void run_button_task()
{
  xTaskCreatePinnedToCore(button_handler, "BUTTON_TASK", 1024, NULL, tskIDLE_PRIORITY, NULL, 1);
}

static void update_button(Button_t *btn)
{
  btn->history = (btn->history << 1) | gpio_get_level(btn->pin);
}

static bool is_button_pressed(Button_t *btn)
{
  if ((btn->history & MASK) == 0b0000000000111111)
  {
    btn->history = 0xffff;
    return 1;
  }
  return 0;
}

static bool is_button_released(Button_t *btn)
{
  if ((btn->history & MASK) == 0b1111000000000000)
  {
    btn->history = 0x0000;
    return true;
  }
  return false;
}

static bool is_button_down(Button_t *btn)
{
  return btn->inverted
             ? is_button_pressed(btn)
             : is_button_released(btn);
}

static bool is_button_up(Button_t *btn)
{
  return btn->inverted
             ? is_button_released(btn)
             : is_button_pressed(btn);
}

static void send_event(gpio_num_t pin, ButtonState_t state)
{
  ButtonEvent_t event = {
      .pin = pin,
      .state = state};

  xQueueSend(buttonQueue, &event, portMAX_DELAY);
}

static void button_handler(void *params)
{
  for (;;)
  {
    for (size_t i = 0; i < buttons_count; i++)
    {
      update_button(&buttons[i]);
      if (is_button_down(&buttons[i]) && buttons[i].down_time == 0)
      {
        ESP_LOGI("*", "%d  DOWN", buttons[i].pin);
        buttons[i].down_time = millis();
        buttons[i].next_long_time = buttons[i].down_time + BUTTON_LONG_PRESS_DURATION_MS;

        send_event(buttons[i].pin, PRESSED);
      }
      else if (is_button_up(&buttons[i]))
      {
        ESP_LOGI("*", "%d  UP", buttons[i].pin);
        buttons[i].down_time = 0;

        send_event(buttons[i].pin, RELEASED);
      }
      else if (buttons[i].down_time && millis() >= buttons[i].next_long_time)
      {
        ESP_LOGI("*", "%d  ON HOLD", buttons[i].pin);
        buttons[i].next_long_time = buttons[i].next_long_time + BUTTON_LONG_PRESS_REPEAT_MS;

        send_event(buttons[i].pin, ON_HOLD);
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}