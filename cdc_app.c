/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "tusb.h"
#include "bsp/board_api.h"

#include "hardware/uart.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

#define CDC_INTERFACE_NUMBER_FOR_UART0  1
#define CDC_INTERFACE_NUMBER_FOR_UART1  3

#define BUFFER_SIZE     65

uint8_t cdc_dev_idx_for_uart0 = 0;
uint8_t cdc_dev_idx_for_uart1 = 0;

void write_uart2cdc(uart_inst_t *uart, uint8_t idx)
{
  uint8_t i = 0;
  uint8_t buf[BUFFER_SIZE];

  while (uart_is_readable(uart) && i < 64) {
    uint8_t ch = uart_getc(uart);
    buf[i] = ch;
    i++;
  }

  tuh_cdc_write(idx, buf, i);
  tuh_cdc_write_flush(idx);
}

void on_uart0_rx()
{
  write_uart2cdc(uart0, cdc_dev_idx_for_uart0);
}

void on_uart1_rx()
{
  write_uart2cdc(uart1, cdc_dev_idx_for_uart1);
}

// Invoked when received new data
void tuh_cdc_rx_cb(uint8_t idx)
{
  tuh_itf_info_t itf_info = { 0 };
  tuh_cdc_itf_get_info(idx, &itf_info);

  uint8_t buf[BUFFER_SIZE]; // +1 for extra null character
  uint32_t const bufsize = sizeof(buf)-1;

  // forward cdc interfaces -> console
  uint32_t count = tuh_cdc_read(idx, buf, bufsize);
  buf[count] = 0;

  if (cdc_dev_idx_for_uart0 == idx) {
    uart_puts(uart0, buf);
  } else if (cdc_dev_idx_for_uart1 == idx) {
    uart_puts(uart1, buf);
  }
}

void tuh_cdc_mount_cb(uint8_t idx)
{
  uint16_t itf_num = 0;

  tuh_itf_info_t itf_info = { 0 };
  tuh_cdc_itf_get_info(idx, &itf_info);

  itf_num = itf_info.desc.bInterfaceNumber;

  printf("CDC Interface is mounted: address = %u, itf_num = %u\r\n",
         itf_info.daddr, itf_num);

  if (CDC_INTERFACE_NUMBER_FOR_UART0 == itf_num) {
    cdc_dev_idx_for_uart0 = idx;
  } else if (CDC_INTERFACE_NUMBER_FOR_UART1 == itf_num) {
    cdc_dev_idx_for_uart1 = idx;
  }

#ifdef CFG_TUH_CDC_LINE_CODING_ON_ENUM
  // CFG_TUH_CDC_LINE_CODING_ON_ENUM must be defined for line coding is set by tinyusb in enumeration
  // otherwise you need to call tuh_cdc_set_line_coding() first
  cdc_line_coding_t line_coding = { 0 };
  if ( tuh_cdc_get_local_line_coding(idx, &line_coding) )
  {
    printf("  Baudrate: %lu, Stop Bits : %u\r\n", line_coding.bit_rate, line_coding.stop_bits);
    printf("  Parity  : %u, Data Width: %u\r\n", line_coding.parity  , line_coding.data_bits);
  }
#endif
}

void tuh_cdc_umount_cb(uint8_t idx)
{
  tuh_itf_info_t itf_info = { 0 };
  tuh_cdc_itf_get_info(idx, &itf_info);

  printf("CDC Interface is unmounted: address = %u, itf_num = %u\r\n", itf_info.daddr, itf_info.desc.bInterfaceNumber);
}
