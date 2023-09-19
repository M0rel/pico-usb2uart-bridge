#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"

#include "pico/time.h"
#include "pico/stdlib.h"

#include "hardware/uart.h"

#define BAUD_RATE       115200
#define UART0_TX_PIN    0
#define UART0_RX_PIN    1

#define UART1_TX_PIN    4
#define UART1_RX_PIN    5

static inline uint32_t board_millis(void) {
  return to_ms_since_boot(get_absolute_time());
}

extern void on_uart0_rx();
extern void on_uart1_rx();

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

void init_pi_uarts()
{
  uart_init(uart0, BAUD_RATE);
  gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

  uart_init(uart1, BAUD_RATE);
  gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
}

int main(void)
{
  board_init();

  printf("TinyUSB Host CDC MSC HID Example\r\n");

  tuh_init(BOARD_TUH_RHPORT);

  init_pi_uarts();

  while (1)
  {
    tuh_task();

    led_blinking_task();
  }

  return 0;
}
