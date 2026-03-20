/*******************************************************************************
 * @file    uart_io.c
 * @brief   TSAKITSAKY ENERGY - Implementation UART
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Driver UART simplifie pour le simulateur Keil Cortex-M0.
 *          En production, adapter les adresses aux registres UART du MCU
 *          cible (ex: STM32F030 USART1).
 ******************************************************************************/
#include "uart_io.h"
#include "config.h"

/*--- Registres UART (adaptes au simulateur Keil) ---*/
#define UART0_DR    (*(volatile uint32_t *)0x40004000)
#define UART0_FR    (*(volatile uint32_t *)0x40004018)
#define UART0_IBRD  (*(volatile uint32_t *)0x40004024)
#define UART0_FBRD  (*(volatile uint32_t *)0x40004028)
#define UART0_LCR   (*(volatile uint32_t *)0x4000402C)
#define UART0_CR    (*(volatile uint32_t *)0x40004030)

#define FR_TXFF     (1U << 5)   /* TX FIFO plein   */
#define FR_RXFE     (1U << 4)   /* RX FIFO vide    */

void uart_init(uint32_t baud_rate) {
    UART0_CR   = 0;
    UART0_IBRD = SYS_CLOCK_HZ / (16 * baud_rate);
    UART0_FBRD = 0;
    UART0_LCR  = (3U << 5) | (1U << 4);  /* 8 bits, FIFO enable */
    UART0_CR   = (1U << 0) | (1U << 8) | (1U << 9);  /* UART + TX + RX */
}

void uart_send_char(char c) {
    while (UART0_FR & FR_TXFF) { /* attendre */ }
    UART0_DR = (uint32_t)c;
}

void uart_send_string(const char *str) {
    while (*str) {
        if (*str == '\n') uart_send_char('\r');
        uart_send_char(*str++);
    }
}

void uart_send_int(int32_t val) {
    char buf[12];
    int i = 0;
    uint32_t uval;

    if (val < 0) {
        uart_send_char('-');
        uval = (uint32_t)(-(val + 1)) + 1U;
    } else {
        uval = (uint32_t)val;
    }

    if (uval == 0) {
        uart_send_char('0');
        return;
    }

    while (uval > 0) {
        buf[i++] = (char)('0' + (uval % 10));
        uval /= 10;
    }
    while (i > 0) {
        uart_send_char(buf[--i]);
    }
}

void uart_send_uint(uint32_t val) {
    uart_send_int((int32_t)val);
}

void uart_send_hex(uint32_t val) {
    static const char hex[] = "0123456789ABCDEF";
    int i;
    uart_send_string("0x");
    for (i = 28; i >= 0; i -= 4) {
        uart_send_char(hex[(val >> i) & 0xF]);
    }
}

char uart_recv_char(void) {
    while (UART0_FR & FR_RXFE) { /* attendre */ }
    return (char)(UART0_DR & 0xFF);
}

uint32_t uart_recv_line(char *buf, uint32_t max_len) {
    uint32_t i = 0;
    while (i < max_len - 1) {
        char c = uart_recv_char();
        if (c == '\r' || c == '\n') {
            uart_send_string("\r\n");
            break;
        }
        if (c == '\b' || c == 0x7F) {
            if (i > 0) {
                i--;
                uart_send_string("\b \b");
            }
            continue;
        }
        buf[i++] = c;
        uart_send_char(c);
    }
    buf[i] = '\0';
    return i;
}
