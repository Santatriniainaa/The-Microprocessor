/*******************************************************************************
 * @file    fsm.h
 * @brief   TSAKITSAKY ENERGY - Machine a etats finis du gestionnaire d'energie
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include "energy.h"
#include "load.h"
#include "alert.h"

typedef enum {
    FSM_STARTUP  = 0,   /* Initialisation, test batterie                    */
    FSM_NORMAL   = 1,   /* Toutes charges ON, production suffisante         */
    FSM_LOW_BAT  = 2,   /* Charge basse prio OFF (SOC < 30%)               */
    FSM_CRITICAL = 3,   /* Eclairage seulement (SOC < 10%)                 */
    FSM_SHUTDOWN = 4,   /* Tout OFF, batterie vide (SOC ~ 0%)              */
    FSM_FAULT    = 5    /* Erreur materielle detectee                       */
} fsm_state_t;

typedef struct {
    fsm_state_t state;         /* etat courant                              */
    fsm_state_t prev_state;    /* etat precedent (pour detecter transitions)*/
    uint32_t    state_timer;   /* ticks passes dans l'etat courant          */
    uint32_t    uptime_s;      /* temps total d'operation en secondes       */
} fsm_context_t;

void         fsm_init(fsm_context_t *fsm);
void         fsm_update(fsm_context_t *fsm, const energy_data_t *energy,
                        load_context_t *loads, alert_context_t *alerts);
const char  *fsm_state_name(fsm_state_t s);

#endif /* FSM_H */
