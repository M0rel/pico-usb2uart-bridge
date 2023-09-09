#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"

#include "pico/time.h"
static inline uint32_t board_millis(void) {
  return to_ms_since_boot(get_absolute_time());
}

void led_blinking_task(void)
{
  const uint32_t interval_ms = 1000;
  static uint32_t start_ms = 0;

  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}


int main(void)
{
  board_init();

  printf("TinyUSB Host CDC MSC HID Example\r\n");

  // init host stack on configured roothub port
  tuh_init(BOARD_TUH_RHPORT);

  while (1)
  {
    tuh_task();

    led_blinking_task();
    cdc_app_task();
  }

  return 0;
}
