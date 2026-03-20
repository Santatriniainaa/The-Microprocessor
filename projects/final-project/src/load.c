/*******************************************************************************
 * @file    load.c
 * @brief   TSAKITSAKY ENERGY - Implementation controle des charges
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Gere 3 charges avec systeme de priorite :
 *          - Load 0 (LOAD1_PIN) : Eclairage LED   → Priorite HAUTE
 *          - Load 1 (LOAD2_PIN) : Chargeur tel    → Priorite MOYENNE
 *          - Load 2 (LOAD3_PIN) : Radio/TV        → Priorite BASSE
 *
 *          La FSM desactive les charges par priorite decroissante
 *          quand la batterie est faible.
 ******************************************************************************/
#include "load.h"

/* Registre GPIO simule - observable dans la fenetre Memory de Keil */
volatile uint32_t LOAD_GPIO_REG = 0;

void load_init(load_context_t *lc) {
    uint32_t i;
    lc->priority[0] = 0;  /* eclairage = priorite haute (derniere a couper) */
    lc->priority[1] = 1;  /* chargeur  = priorite moyenne                   */
    lc->priority[2] = 2;  /* radio/TV  = priorite basse (premiere a couper)  */

    for (i = 0; i < NUM_LOADS; i++) {
        lc->enabled[i] = 0;
    }
    lc->gpio_reg = 0;
    LOAD_GPIO_REG = 0;
}

void load_set(load_context_t *lc, uint32_t id, uint32_t on) {
    if (id < NUM_LOADS) {
        lc->enabled[id] = on ? 1 : 0;
    }
}

void load_set_all(load_context_t *lc, uint32_t l0, uint32_t l1, uint32_t l2) {
    lc->enabled[0] = l0 ? 1 : 0;
    lc->enabled[1] = l1 ? 1 : 0;
    lc->enabled[2] = l2 ? 1 : 0;
}

void load_apply(load_context_t *lc) {
    uint32_t reg = 0;

    if (lc->enabled[0]) reg |= (1U << LOAD1_PIN);
    if (lc->enabled[1]) reg |= (1U << LOAD2_PIN);
    if (lc->enabled[2]) reg |= (1U << LOAD3_PIN);

    lc->gpio_reg = reg;
    LOAD_GPIO_REG = reg;
}

uint32_t load_count_active(const load_context_t *lc) {
    uint32_t count = 0, i;
    for (i = 0; i < NUM_LOADS; i++) {
        if (lc->enabled[i]) count++;
    }
    return count;
}
