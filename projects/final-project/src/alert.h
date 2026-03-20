/*******************************************************************************
 * @file    alert.h
 * @brief   TSAKITSAKY ENERGY - Gestion des alertes et LEDs de statut
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef ALERT_H
#define ALERT_H

#include <stdint.h>

#define LED_GREEN    0
#define LED_ORANGE   1
#define LED_RED      2
#define NUM_LEDS     3

typedef struct {
    uint32_t led_state[NUM_LEDS];   /* etat de chaque LED                  */
    uint32_t gpio_led_reg;          /* registre GPIO LED simule            */
} alert_context_t;

extern volatile uint32_t ALERT_LED_REG;  /* observable Memory Window       */

void alert_init(alert_context_t *a);
void alert_set_led(alert_context_t *a, uint32_t led, uint32_t on);
void alert_apply(alert_context_t *a);
void alert_all_off(alert_context_t *a);

#endif /* ALERT_H */
