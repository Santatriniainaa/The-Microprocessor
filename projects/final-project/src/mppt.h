/*******************************************************************************
 * @file    mppt.h
 * @brief   TSAKITSAKY ENERGY - Algorithme MPPT (Maximum Power Point Tracking)
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Algorithme Perturb & Observe pour maximiser la puissance extraite
 *          du panneau solaire en ajustant le duty cycle du convertisseur DC-DC.
 ******************************************************************************/
#ifndef MPPT_H
#define MPPT_H

#include <stdint.h>

typedef struct {
    uint32_t duty_percent;     /* duty cycle actuel (MPPT_DUTY_MIN..MAX)    */
    uint32_t p_previous_mw;    /* puissance a l'iteration precedente        */
    int32_t  direction;        /* +1 ou -1 (sens de perturbation)           */
    uint32_t update_count;     /* nombre d'ajustements effectues            */
    uint32_t pwm_reg;          /* registre PWM simule                       */
} mppt_context_t;

extern volatile uint32_t MPPT_PWM_REG;   /* observable Memory Window       */

void mppt_init(mppt_context_t *m);
void mppt_update(mppt_context_t *m, uint32_t v_panel_mv, uint32_t i_panel_ma);
uint32_t mppt_get_duty(const mppt_context_t *m);

#endif /* MPPT_H */
