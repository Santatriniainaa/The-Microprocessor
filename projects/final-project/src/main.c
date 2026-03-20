/*******************************************************************************
 * @file    main.c
 * @brief   TSAKITSAKY ENERGY - Programme principal
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 * @version 1.0.0
 *
 * @details Point d'entree du firmware TSAKITSAKY ENERGY.
 *          Orchestre tous les modules selon le chronogramme suivant :
 *
 *          SysTick ISR (1 kHz) → flags temporels
 *          Boucle principale :
 *            - 10 Hz : ADC → Energy → MPPT → FSM → Loads/Alerts
 *            -  1 Hz : Display status UART
 *            - 1/60 Hz : Logger (historique)
 *
 * @team    Architecture  : S. RAZAFINDRAKOTO (Firmware Lead)
 *          ADC/Energy    : S. RAZAFINDRAKOTO (Firmware Lead)
 *          FSM/Loads     : S. RAZAFINDRAKOTO (Firmware Lead)
 *          MPPT          : S. RAZAFINDRAKOTO (Firmware Lead)
 *          UART/Display  : S. RAZAFINDRAKOTO (Firmware Lead)
 *          Logger        : S. RAZAFINDRAKOTO (Firmware Lead)
 *          Tests         : S. RAZAFINDRAKOTO (Firmware Lead)
 *          Business Plan : S. RAZAFINDRAKOTO (Business Analyst)
 *          Hardware      : S. RAZAFINDRAKOTO (Electronics Eng.)
 *
 * @note    Compile avec Keil MDK pour ARM Cortex-M0 (simulateur).
 *          Pour le deploiement reel, adapter les adresses des registres
 *          peripheriques au MCU cible (ex: STM32F030).
 ******************************************************************************/

#include "config.h"
#include "adc.h"
#include "energy.h"
#include "load.h"
#include "alert.h"
#include "fsm.h"
#include "mppt.h"
#include "logger.h"
#include "display.h"
#include "uart_io.h"

/*===========================================================================
 * CONTEXTES GLOBAUX (un par module)
 *=========================================================================*/
static adc_context_t      g_adc;
static energy_data_t      g_energy;
static load_context_t     g_loads;
static alert_context_t    g_alerts;
static fsm_context_t      g_fsm;
static mppt_context_t     g_mppt;
static logger_context_t   g_logger;

/*===========================================================================
 * DRAPEAUX TEMPORELS (mis a jour par SysTick ISR)
 *=========================================================================*/
static volatile uint32_t  g_tick_ms    = 0;
static volatile uint32_t  g_flag_10hz  = 0;   /* toutes les 100 ms        */
static volatile uint32_t  g_flag_1hz   = 0;   /* toutes les 1 s           */
static volatile uint32_t  g_flag_mppt  = 0;   /* toutes les 10 s          */
static volatile uint32_t  g_flag_log   = 0;   /* toutes les 60 s          */

/*===========================================================================
 * HANDLER SYSTICK (interruption 1 kHz)
 *=========================================================================*/
void SysTick_Handler(void) {
    g_tick_ms++;

    /* Flag 10 Hz (100 ms) */
    if ((g_tick_ms % (TICK_HZ / CONTROL_LOOP_HZ)) == 0) {
        g_flag_10hz = 1;
    }

    /* Flag 1 Hz (1000 ms) */
    if ((g_tick_ms % TICK_HZ) == 0) {
        g_flag_1hz = 1;
    }

    /* Flag MPPT (toutes les MPPT_INTERVAL_S secondes) */
    if ((g_tick_ms % (TICK_HZ * MPPT_INTERVAL_S)) == 0) {
        g_flag_mppt = 1;
    }

    /* Flag Logger (toutes les LOG_INTERVAL_S secondes) */
    if ((g_tick_ms % (TICK_HZ * LOG_INTERVAL_S)) == 0) {
        g_flag_log = 1;
    }
}

/*===========================================================================
 * FONCTION PRINCIPALE
 *=========================================================================*/
int main(void) {

    /* =====================================================
     * PHASE 1 : INITIALISATION DE TOUS LES MODULES
     * ====================================================*/
    uart_init(UART_BAUD_RATE);
    adc_init(&g_adc);
    energy_init(&g_energy);
    load_init(&g_loads);
    alert_init(&g_alerts);
    fsm_init(&g_fsm);
    mppt_init(&g_mppt);
    logger_init(&g_logger);

    /* Afficher le banner de demarrage */
    display_banner();
    uart_send_string("Initialisation des modules... OK\r\n");
    uart_send_string("Configuration SysTick 1 kHz... ");

    /* =====================================================
     * PHASE 2 : CONFIGURER LE TIMER SYSTICK
     * ====================================================*/
    SYSTICK_RVR = (SYS_CLOCK_HZ / TICK_HZ) - 1;   /* 11999 pour 1 ms     */
    SYSTICK_CVR = 0;                                 /* reset compteur      */
    SYSTICK_CSR = 0x07;                              /* Enable+IRQ+CPUclk   */

    uart_send_string("OK\r\n");
    uart_send_string("Systeme pret. Demarrage...\r\n\r\n");

    /* =====================================================
     * PHASE 3 : BOUCLE PRINCIPALE (ne sort jamais)
     * ====================================================*/
    while (1) {

        /* -------------------------------------------------
         * BOUCLE DE CONTROLE A 10 Hz (toutes les 100 ms)
         * Pipeline : ADC → Energy → FSM → Loads/Alerts
         * ------------------------------------------------*/
        if (g_flag_10hz) {
            g_flag_10hz = 0;

            /* Etape 1 : Echantillonner les capteurs */
            adc_sample(&g_adc);

            /* Etape 2 : Calculer grandeurs energetiques */
            energy_update(&g_energy, &g_adc);

            /* Etape 3 : Mettre a jour la FSM
             * (decide l'etat, commande les charges et alertes) */
            fsm_update(&g_fsm, &g_energy, &g_loads, &g_alerts);
        }

        /* -------------------------------------------------
         * MISE A JOUR MPPT (toutes les 10 s)
         * ------------------------------------------------*/
        if (g_flag_mppt) {
            g_flag_mppt = 0;
            mppt_update(&g_mppt, g_energy.v_panel_mv, g_energy.i_panel_ma);
        }

        /* -------------------------------------------------
         * AFFICHAGE UART A 1 Hz (toutes les 1 s)
         * ------------------------------------------------*/
        if (g_flag_1hz) {
            g_flag_1hz = 0;
            g_fsm.uptime_s++;
            display_status(&g_fsm, &g_energy, &g_loads);
        }

        /* -------------------------------------------------
         * LOGGER : enregistrement historique (toutes les 60 s)
         * ------------------------------------------------*/
        if (g_flag_log) {
            g_flag_log = 0;

            log_entry_t entry;
            entry.timestamp_s  = g_fsm.uptime_s;
            entry.soc_percent  = g_energy.soc_percent;
            entry.p_solar_mw   = g_energy.p_solar_mw;
            entry.p_load_mw    = g_energy.p_load_mw;
            entry.state        = (uint8_t)g_fsm.state;
            entry.loads_active = (uint8_t)load_count_active(&g_loads);

            logger_add(&g_logger, &entry);
        }

        /* -------------------------------------------------
         * COMMANDES UART (si caractere disponible)
         * Non-bloquant : verifie le flag RXFE
         * ------------------------------------------------*/
        {
            volatile uint32_t *uart_fr = (volatile uint32_t *)0x40004018;
            if (!(*uart_fr & (1U << 4))) {
                volatile uint32_t *uart_dr = (volatile uint32_t *)0x40004000;
                char cmd = (char)(*uart_dr & 0xFF);

                switch (cmd) {
                case 'd': case 'D':
                    /* Diagnostic detaille */
                    display_detailed(&g_fsm, &g_energy, &g_loads);
                    uart_send_string("MPPT duty: ");
                    uart_send_uint(mppt_get_duty(&g_mppt));
                    uart_send_string("%\r\n");
                    break;
                case 'l': case 'L':
                    /* Exporter le log CSV */
                    logger_export_csv(&g_logger);
                    break;
                case 's': case 'S':
                    /* Resume du logger */
                    logger_print_summary(&g_logger);
                    break;
                case 'h': case 'H': case '?':
                    /* Aide */
                    uart_send_string("\r\n--- COMMANDES ---\r\n");
                    uart_send_string("  d = Diagnostic detaille\r\n");
                    uart_send_string("  l = Exporter log CSV\r\n");
                    uart_send_string("  s = Resume statistiques\r\n");
                    uart_send_string("  h = Cette aide\r\n");
                    uart_send_string("-----------------\r\n");
                    break;
                default:
                    break;
                }
            }
        }
    }

    /* Ne devrait jamais arriver ici */
    return 0;
}
