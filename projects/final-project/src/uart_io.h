/*******************************************************************************
 * @file    uart_io.h
 * @brief   TSAKITSAKY ENERGY - Interface UART (entrees/sorties serie)
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef UART_IO_H
#define UART_IO_H

#include <stdint.h>

void     uart_init(uint32_t baud_rate);
void     uart_send_char(char c);
void     uart_send_string(const char *str);
void     uart_send_int(int32_t val);
void     uart_send_uint(uint32_t val);
void     uart_send_hex(uint32_t val);
char     uart_recv_char(void);
uint32_t uart_recv_line(char *buf, uint32_t max_len);

#endif /* UART_IO_H */
