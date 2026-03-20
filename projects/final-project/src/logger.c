/*******************************************************************************
 * @file    logger.c
 * @brief   TSAKITSAKY ENERGY - Implementation historique (ring buffer)
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Stocke les 128 derniers enregistrements energetiques (1 par minute
 *          = ~2 heures d'historique). Exporte en format CSV via UART pour
 *          analyse sur PC ou transmission SMS.
 ******************************************************************************/
#include "logger.h"
#include "uart_io.h"

void logger_init(logger_context_t *log) {
    log->head         = 0;
    log->count        = 0;
    log->total_logged = 0;
}

void logger_add(logger_context_t *log, const log_entry_t *entry) {
    log->entries[log->head] = *entry;
    log->head = (log->head + 1) % LOG_BUFFER_SIZE;
    if (log->count < LOG_BUFFER_SIZE) {
        log->count++;
    }
    log->total_logged++;
}

void logger_export_csv(const logger_context_t *log) {
    uint32_t i, idx;

    uart_send_string("timestamp_s,soc_pct,p_solar_mw,p_load_mw,state,loads\r\n");

    /* Parcourir du plus ancien au plus recent */
    uint32_t start;
    if (log->count < LOG_BUFFER_SIZE) {
        start = 0;
    } else {
        start = log->head;  /* le plus ancien est juste apres head */
    }

    for (i = 0; i < log->count; i++) {
        idx = (start + i) % LOG_BUFFER_SIZE;
        const log_entry_t *e = &log->entries[idx];

        uart_send_uint(e->timestamp_s);
        uart_send_char(',');
        uart_send_uint(e->soc_percent);
        uart_send_char(',');
        uart_send_uint(e->p_solar_mw);
        uart_send_char(',');
        uart_send_uint(e->p_load_mw);
        uart_send_char(',');
        uart_send_uint((uint32_t)e->state);
        uart_send_char(',');
        uart_send_uint((uint32_t)e->loads_active);
        uart_send_string("\r\n");
    }

    uart_send_string("--- ");
    uart_send_uint(log->count);
    uart_send_string(" entrees exportees ---\r\n");
}

void logger_print_summary(const logger_context_t *log) {
    if (log->count == 0) {
        uart_send_string("Logger: aucune donnee.\r\n");
        return;
    }

    /* Calculer min/max/moyenne du SOC et de la puissance solaire */
    uint32_t soc_min = 100, soc_max = 0;
    uint32_t psol_max = 0;
    uint32_t soc_sum = 0;
    uint32_t i, idx;

    uint32_t start = (log->count < LOG_BUFFER_SIZE) ? 0 : log->head;

    for (i = 0; i < log->count; i++) {
        idx = (start + i) % LOG_BUFFER_SIZE;
        const log_entry_t *e = &log->entries[idx];

        if (e->soc_percent < soc_min) soc_min = e->soc_percent;
        if (e->soc_percent > soc_max) soc_max = e->soc_percent;
        if (e->p_solar_mw > psol_max) psol_max = e->p_solar_mw;
        soc_sum += e->soc_percent;
    }

    uart_send_string("\r\n=== RESUME HISTORIQUE ===\r\n");
    uart_send_string("Entrees     : ");
    uart_send_uint(log->count);
    uart_send_string(" / ");
    uart_send_uint(LOG_BUFFER_SIZE);
    uart_send_string("\r\n");

    uart_send_string("Total logged: ");
    uart_send_uint(log->total_logged);
    uart_send_string("\r\n");

    uart_send_string("SOC min     : ");
    uart_send_uint(soc_min);
    uart_send_string("%\r\n");

    uart_send_string("SOC max     : ");
    uart_send_uint(soc_max);
    uart_send_string("%\r\n");

    uart_send_string("SOC moyen   : ");
    uart_send_uint(soc_sum / log->count);
    uart_send_string("%\r\n");

    uart_send_string("P solar max : ");
    uart_send_uint(psol_max);
    uart_send_string(" mW\r\n");

    uart_send_string("=========================\r\n");
}
