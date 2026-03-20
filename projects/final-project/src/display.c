/*******************************************************************************
 * @file    display.c
 * @brief   TSAKITSAKY ENERGY - Implementation affichage UART
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Formate et affiche les donnees systeme sur la sortie UART.
 *          Compatible avec un terminal serie (PuTTY, minicom) ou un
 *          ecran LCD 16x2 via adaptateur UART-I2C.
 ******************************************************************************/
#include "display.h"
#include "uart_io.h"

void display_banner(void) {
    uart_send_string("\r\n");
    uart_send_string("========================================\r\n");
    uart_send_string("  TSAKITSAKY ENERGY v1.0\r\n");
    uart_send_string("  Controleur Solaire Intelligent\r\n");
    uart_send_string("  ARM Cortex-M0\r\n");
    uart_send_string("  (c) 2026 S. RAZAFINDRAKOTO\r\n");
    uart_send_string("========================================\r\n");
    uart_send_string("\r\n");
}

/*---------------------------------------------------------------------------
 * Affichage compact (1 ligne par seconde) - mode normal
 *-------------------------------------------------------------------------*/
void display_status(const fsm_context_t *fsm, const energy_data_t *e,
                    const load_context_t *loads) {
    /* Format: [120s] NORMAL   | Bat:12.5V 85% | Sol:38.2W | Ld:18.3W | L:111 */

    uart_send_char('[');
    uart_send_int((int32_t)fsm->uptime_s);
    uart_send_string("s] ");

    uart_send_string(fsm_state_name(fsm->state));

    uart_send_string(" | Bat:");
    uart_send_int((int32_t)(e->v_battery_mv / 100));
    uart_send_char('.');
    uart_send_int((int32_t)((e->v_battery_mv / 10) % 10));
    uart_send_string("V ");
    uart_send_int((int32_t)e->soc_percent);
    uart_send_char('%');

    uart_send_string(" | Sol:");
    uart_send_int((int32_t)(e->p_solar_mw / 1000));
    uart_send_char('.');
    uart_send_int((int32_t)((e->p_solar_mw / 100) % 10));
    uart_send_char('W');

    uart_send_string(" | Ld:");
    uart_send_int((int32_t)(e->p_load_mw / 1000));
    uart_send_char('.');
    uart_send_int((int32_t)((e->p_load_mw / 100) % 10));
    uart_send_char('W');

    uart_send_string(" | L:");
    uart_send_char(loads->enabled[0] ? '1' : '0');
    uart_send_char(loads->enabled[1] ? '1' : '0');
    uart_send_char(loads->enabled[2] ? '1' : '0');

    if (e->is_charging) {
        uart_send_string(" CHG");
    }

    uart_send_string("\r\n");
}

/*---------------------------------------------------------------------------
 * Affichage detaille (multi-lignes) - mode diagnostic
 *-------------------------------------------------------------------------*/
void display_detailed(const fsm_context_t *fsm, const energy_data_t *e,
                      const load_context_t *loads) {
    uart_send_string("\r\n--- DIAGNOSTIC DETAILLE ---\r\n");

    uart_send_string("Uptime       : ");
    uart_send_int((int32_t)fsm->uptime_s);
    uart_send_string(" s\r\n");

    uart_send_string("Etat         : ");
    uart_send_string(fsm_state_name(fsm->state));
    uart_send_string("\r\n");

    uart_send_string("V panneau    : ");
    uart_send_int((int32_t)e->v_panel_mv);
    uart_send_string(" mV\r\n");

    uart_send_string("V batterie   : ");
    uart_send_int((int32_t)e->v_battery_mv);
    uart_send_string(" mV\r\n");

    uart_send_string("I panneau    : ");
    uart_send_int((int32_t)e->i_panel_ma);
    uart_send_string(" mA\r\n");

    uart_send_string("I charges    : ");
    uart_send_int((int32_t)e->i_load_ma);
    uart_send_string(" mA\r\n");

    uart_send_string("P solaire    : ");
    uart_send_int((int32_t)e->p_solar_mw);
    uart_send_string(" mW\r\n");

    uart_send_string("P charges    : ");
    uart_send_int((int32_t)e->p_load_mw);
    uart_send_string(" mW\r\n");

    uart_send_string("P net        : ");
    uart_send_int(e->p_net_mw);
    uart_send_string(" mW\r\n");

    uart_send_string("SOC          : ");
    uart_send_int((int32_t)e->soc_percent);
    uart_send_string(" %\r\n");

    uart_send_string("Energie jour : ");
    uart_send_int((int32_t)e->energy_today_wh);
    uart_send_string(" Wh\r\n");

    uart_send_string("Charges      : ");
    uart_send_string(loads->enabled[0] ? "ECL=ON " : "ECL=OFF ");
    uart_send_string(loads->enabled[1] ? "CHG=ON " : "CHG=OFF ");
    uart_send_string(loads->enabled[2] ? "TV=ON"   : "TV=OFF");
    uart_send_string("\r\n");

    uart_send_string("GPIO loads   : ");
    uart_send_hex(loads->gpio_reg);
    uart_send_string("\r\n");

    uart_send_string("---------------------------\r\n");
}
