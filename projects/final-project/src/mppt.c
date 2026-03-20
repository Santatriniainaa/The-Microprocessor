/*******************************************************************************
 * @file    mppt.c
 * @brief   TSAKITSAKY ENERGY - Implementation MPPT Perturb & Observe
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Algorithme P&O :
 *   1. Mesurer la puissance actuelle P = V * I
 *   2. Comparer avec la puissance precedente
 *   3. Si P a augmente : continuer dans la meme direction
 *      Si P a diminue  : inverser la direction
 *   4. Ajuster le duty cycle du PWM de ±1%
 *
 *   Cet algorithme simple est tres utilise dans l'industrie solaire.
 *   Il converge vers le point de puissance maximale (MPP) du panneau.
 ******************************************************************************/
#include "mppt.h"
#include "config.h"

volatile uint32_t MPPT_PWM_REG = 0;

void mppt_init(mppt_context_t *m) {
    m->duty_percent = MPPT_DUTY_INIT;
    m->p_previous_mw = 0;
    m->direction = 1;          /* commencer par augmenter */
    m->update_count = 0;
    m->pwm_reg = MPPT_DUTY_INIT;
    MPPT_PWM_REG = MPPT_DUTY_INIT;
}

void mppt_update(mppt_context_t *m, uint32_t v_panel_mv, uint32_t i_panel_ma) {
    /* Calculer la puissance actuelle */
    uint32_t p_current_mw = (v_panel_mv * i_panel_ma) / 1000;

    /* Algorithme Perturb & Observe */
    if (p_current_mw > m->p_previous_mw) {
        /* La puissance a augmente : continuer dans la meme direction */
        /* direction reste inchangee */
    } else if (p_current_mw < m->p_previous_mw) {
        /* La puissance a diminue : inverser la direction */
        m->direction = -(m->direction);
    }
    /* Si egal : ne rien changer */

    /* Appliquer la perturbation */
    if (m->direction > 0) {
        if (m->duty_percent < MPPT_DUTY_MAX) {
            m->duty_percent += MPPT_STEP;
        }
    } else {
        if (m->duty_percent > MPPT_DUTY_MIN) {
            m->duty_percent -= MPPT_STEP;
        }
    }

    /* Sauvegarder pour la prochaine iteration */
    m->p_previous_mw = p_current_mw;
    m->update_count++;

    /* Mettre a jour le registre PWM */
    m->pwm_reg = m->duty_percent;
    MPPT_PWM_REG = m->duty_percent;
}

uint32_t mppt_get_duty(const mppt_context_t *m) {
    return m->duty_percent;
}
