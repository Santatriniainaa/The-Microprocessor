/*******************************************************************************
 * @file    logger.h
 * @brief   TSAKITSAKY ENERGY - Historique energie (ring buffer)
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "config.h"

typedef struct {
    uint32_t timestamp_s;       /* horodatage                               */
    uint32_t soc_percent;       /* SOC au moment du log                     */
    uint32_t p_solar_mw;        /* puissance solaire                        */
    uint32_t p_load_mw;         /* puissance consommee                      */
    uint8_t  state;             /* etat FSM                                 */
    uint8_t  loads_active;      /* nombre de charges actives                */
} log_entry_t;

typedef struct {
    log_entry_t entries[LOG_BUFFER_SIZE];
    uint32_t    head;            /* index ecriture                          */
    uint32_t    count;           /* nombre d'entrees valides                */
    uint32_t    total_logged;    /* total depuis le demarrage               */
} logger_context_t;

void logger_init(logger_context_t *log);
void logger_add(logger_context_t *log, const log_entry_t *entry);
void logger_export_csv(const logger_context_t *log);
void logger_print_summary(const logger_context_t *log);

#endif /* LOGGER_H */
