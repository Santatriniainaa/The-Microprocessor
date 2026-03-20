/*******************************************************************************
 * @file    alert.c
 * @brief   TSAKITSAKY ENERGY - Implementation alertes et LEDs
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details 3 LEDs de statut :
 *          - VERTE   (bit 0) : Fonctionnement normal
 *          - ORANGE  (bit 1) : Batterie faible / avertissement
 *          - ROUGE   (bit 2) : Critique / alarme
 ******************************************************************************/
#include "alert.h"

volatile uint32_t ALERT_LED_REG = 0;

void alert_init(alert_context_t *a) {
    uint32_t i;
    for (i = 0; i < NUM_LEDS; i++) {
        a->led_state[i] = 0;
    }
    a->gpio_led_reg = 0;
    ALERT_LED_REG = 0;
}

void alert_set_led(alert_context_t *a, uint32_t led, uint32_t on) {
    if (led < NUM_LEDS) {
        a->led_state[led] = on ? 1 : 0;
    }
}

void alert_all_off(alert_context_t *a) {
    uint32_t i;
    for (i = 0; i < NUM_LEDS; i++) {
        a->led_state[i] = 0;
    }
}

void alert_apply(alert_context_t *a) {
    uint32_t reg = 0;

    if (a->led_state[LED_GREEN])  reg |= (1U << 0);
    if (a->led_state[LED_ORANGE]) reg |= (1U << 1);
    if (a->led_state[LED_RED])    reg |= (1U << 2);

    a->gpio_led_reg = reg;
    ALERT_LED_REG = reg;
}
