/*******************************************************************************
 * @file    fsm.c
 * @brief   TSAKITSAKY ENERGY - Implementation FSM gestionnaire d'energie
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Machine a 6 etats avec hysteresis pour eviter les oscillations :
 *
 *   STARTUP ──→ NORMAL ──→ LOW_BAT ──→ CRITICAL ──→ SHUTDOWN
 *                  ↑           │           │             │
 *                  └─ SOC>40% ─┘           │             │
 *                              ↑           │             │
 *                              └─ SOC>20% ─┘             │
 *                                          ↑             │
 *                                          └─ soleil+bat─┘
 ******************************************************************************/
#include "fsm.h"
#include "config.h"

static const char *state_names[] = {
    "STARTUP ", "NORMAL  ", "LOW_BAT ", "CRITICAL", "SHUTDOWN", "FAULT   "
};

const char *fsm_state_name(fsm_state_t s) {
    return (s <= FSM_FAULT) ? state_names[s] : "UNKNOWN ";
}

void fsm_init(fsm_context_t *fsm) {
    fsm->state       = FSM_STARTUP;
    fsm->prev_state  = FSM_STARTUP;
    fsm->state_timer = 0;
    fsm->uptime_s    = 0;
}

void fsm_update(fsm_context_t *fsm, const energy_data_t *energy,
                load_context_t *loads, alert_context_t *alerts) {

    fsm->state_timer++;
    fsm_state_t next = fsm->state;
    uint32_t soc  = energy->soc_percent;
    uint32_t vbat = energy->v_battery_mv;

    switch (fsm->state) {

    /*--- STARTUP : 2 secondes d'initialisation puis evaluation ---*/
    case FSM_STARTUP:
        load_set_all(loads, 0, 0, 0);
        alert_all_off(alerts);
        alert_set_led(alerts, LED_GREEN, (fsm->state_timer >> 2) & 1);

        if (fsm->state_timer > 20) {
            if (vbat > BAT_SHUTDOWN_MV) {
                next = (soc > 30) ? FSM_NORMAL : FSM_LOW_BAT;
            } else {
                next = FSM_SHUTDOWN;
            }
        }
        break;

    /*--- NORMAL : toutes charges actives ---*/
    case FSM_NORMAL:
        load_set_all(loads, 1, 1, 1);
        alert_all_off(alerts);
        alert_set_led(alerts, LED_GREEN, 1);

        /* Alerte surcharge batterie */
        if (vbat > BAT_OVERCHARGE_MV) {
            alert_set_led(alerts, LED_ORANGE, 1);
        }
        /* Transitions de degradation */
        if (soc < 10) {
            next = FSM_CRITICAL;
        } else if (soc < 30) {
            next = FSM_LOW_BAT;
        }
        break;

    /*--- LOW_BAT : couper charge basse priorite ---*/
    case FSM_LOW_BAT:
        load_set_all(loads, 1, 1, 0);  /* Radio/TV OFF */
        alert_all_off(alerts);
        alert_set_led(alerts, LED_ORANGE, 1);

        /* Remontee avec hysteresis (+10%) */
        if (soc > 40) {
            next = FSM_NORMAL;
        }
        /* Degradation */
        if (soc < 10) {
            next = FSM_CRITICAL;
        }
        break;

    /*--- CRITICAL : eclairage seulement ---*/
    case FSM_CRITICAL:
        load_set_all(loads, 1, 0, 0);  /* Seul eclairage */
        alert_all_off(alerts);
        alert_set_led(alerts, LED_RED, 1);

        /* Remontee avec hysteresis */
        if (soc > 20) {
            next = FSM_LOW_BAT;
        }
        /* Arret total si batterie trop basse */
        if (vbat < BAT_SHUTDOWN_MV) {
            next = FSM_SHUTDOWN;
        }
        break;

    /*--- SHUTDOWN : tout eteint, economie maximale ---*/
    case FSM_SHUTDOWN:
        load_set_all(loads, 0, 0, 0);
        alert_all_off(alerts);
        /* LED rouge clignotante lente */
        alert_set_led(alerts, LED_RED, (fsm->state_timer >> 3) & 1);

        /* Reveil si panneau produit et batterie remonte */
        if (energy->p_solar_mw > 1000 && vbat > BAT_CRITICAL_MV) {
            next = FSM_CRITICAL;
        }
        break;

    /*--- FAULT : erreur materielle ---*/
    case FSM_FAULT:
        load_set_all(loads, 0, 0, 0);
        alert_all_off(alerts);
        alert_set_led(alerts, LED_RED, 1);
        alert_set_led(alerts, LED_ORANGE, 1);
        /* Reste en FAULT jusqu'a reset manuel */
        break;
    }

    /* Transition d'etat */
    if (next != fsm->state) {
        fsm->prev_state  = fsm->state;
        fsm->state       = next;
        fsm->state_timer = 0;
    }

    /* Appliquer les commandes aux actionneurs */
    load_apply(loads);
    alert_apply(alerts);
}
