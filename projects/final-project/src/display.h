/*******************************************************************************
 * @file    display.h
 * @brief   TSAKITSAKY ENERGY - Affichage et reporting via UART
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "fsm.h"
#include "energy.h"
#include "load.h"

void display_banner(void);
void display_status(const fsm_context_t *fsm, const energy_data_t *e,
                    const load_context_t *loads);
void display_detailed(const fsm_context_t *fsm, const energy_data_t *e,
                      const load_context_t *loads);

#endif /* DISPLAY_H */
