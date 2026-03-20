/*******************************************************************************
 * @file    load.h
 * @brief   TSAKITSAKY ENERGY - Controle des charges par priorite
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef LOAD_H
#define LOAD_H

#include <stdint.h>
#include "config.h"

typedef struct {
    uint32_t enabled[NUM_LOADS];     /* 0=OFF, 1=ON par charge              */
    uint32_t priority[NUM_LOADS];    /* 0=haute, 1=moyenne, 2=basse         */
    uint32_t gpio_reg;               /* registre GPIO simule                */
} load_context_t;

extern volatile uint32_t LOAD_GPIO_REG;  /* observable dans Memory Window   */

void load_init(load_context_t *lc);
void load_set(load_context_t *lc, uint32_t load_id, uint32_t on);
void load_set_all(load_context_t *lc, uint32_t l0, uint32_t l1, uint32_t l2);
void load_apply(load_context_t *lc);
uint32_t load_count_active(const load_context_t *lc);

#endif /* LOAD_H */
