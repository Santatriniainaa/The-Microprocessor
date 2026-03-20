/*******************************************************************************
 * @file    config.h
 * @brief   TSAKITSAKY ENERGY - Configuration systeme et seuils
 * @author  Santatriniaina RAZAFINDRAKOTO
 * @date    20 mars 2026
 *
 * @details Ce fichier centralise toutes les constantes de configuration du
 *          systeme. Modifier les seuils ici pour adapter le comportement a
 *          differentes configurations panneau/batterie.
 *
 * @team    Firmware Lead: S. RAZAFINDRAKOTO
 ******************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/*===========================================================================
 * PARAMETRES SYSTEME
 *=========================================================================*/
#define FW_VERSION_MAJOR     1
#define FW_VERSION_MINOR     0
#define FW_VERSION_PATCH     0

#define SYS_CLOCK_HZ        12000000   /* 12 MHz (simulateur Keil)          */
#define TICK_HZ              1000       /* SysTick a 1 kHz (1 ms)           */
#define CONTROL_LOOP_HZ      10         /* Boucle de controle 10 Hz (100ms) */
#define DISPLAY_HZ            1         /* Rafraichissement affichage 1 Hz  */
#define LOG_INTERVAL_S       60         /* Log dans historique toutes 60 s  */
#define MPPT_INTERVAL_S      10         /* Ajustement MPPT toutes les 10 s  */

/*===========================================================================
 * SEUILS BATTERIE PLOMB 12V (millivolts)
 * Basees sur courbe OCV (Open Circuit Voltage) batterie plomb-acide
 *=========================================================================*/
#define BAT_FULL_MV          12700      /* 12.70V = 100% SOC                */
#define BAT_NORMAL_MV        12200      /* 12.20V = 60%  SOC                */
#define BAT_LOW_MV           11750      /* 11.75V = 30%  SOC                */
#define BAT_CRITICAL_MV      11310      /* 11.31V = 10%  SOC                */
#define BAT_SHUTDOWN_MV      10500      /* 10.50V = 0%   SOC (arret)        */
#define BAT_OVERCHARGE_MV    14400      /* 14.4V  seuil surcharge           */
#define BAT_FLOAT_MV         13600      /* 13.6V  tension de floating       */

/*===========================================================================
 * SEUILS COURANT (milliamperes)
 *=========================================================================*/
#define MAX_LOAD_CURRENT_MA  8000       /* 8A max total toutes charges      */
#define MAX_SINGLE_LOAD_MA   5000       /* 5A max par charge individuelle   */
#define MIN_CHARGE_CURRENT_MA  50       /* 50mA min pour considerer charge  */

/*===========================================================================
 * SEUILS TEMPERATURE (degres Celsius x10 pour precision)
 *=========================================================================*/
#define TEMP_NOMINAL         250        /* 25.0°C nominal                   */
#define TEMP_WARNING         550        /* 55.0°C alerte                    */
#define TEMP_SHUTDOWN        650        /* 65.0°C arret d'urgence           */

/*===========================================================================
 * ADC CONFIGURATION
 *=========================================================================*/
#define ADC_RESOLUTION       4096       /* 12 bits                          */
#define ADC_VREF_MV          3300       /* 3.3V reference                   */
#define ADC_NUM_CHANNELS     4
#define ADC_FILTER_SIZE      8          /* Moyennage glissant sur 8 pts     */

/* Canaux ADC */
#define ADC_CH_VPANEL        0          /* Tension panneau solaire          */
#define ADC_CH_VBAT          1          /* Tension batterie                 */
#define ADC_CH_IPANEL        2          /* Courant panneau                  */
#define ADC_CH_ILOAD         3          /* Courant total des charges        */

/* Facteurs de conversion ADC → tension reelle (millivolts)
 * V_reel(mV) = ADC_raw * FACTOR / ADC_RESOLUTION
 * Calcul base sur les diviseurs resistifs du hardware design             */
#define VPANEL_FACTOR        7800       /* R1=68k, R2=10k → 0-25.7V        */
#define VBAT_FACTOR          4900       /* R3=39k, R4=10k → 0-16.2V        */

/* Facteur courant INA219 (mA par bit ADC) */
#define IPANEL_FACTOR_MA     1          /* 1 mA/bit (simplifie pour sim)    */
#define ILOAD_FACTOR_MA      1

/*===========================================================================
 * CHARGES (LOADS) - GPIO
 *=========================================================================*/
#define NUM_LOADS            3
#define LOAD1_PIN            5          /* Eclairage LED - Priorite HAUTE   */
#define LOAD2_PIN            6          /* Chargeur telephone - Priorite MOY*/
#define LOAD3_PIN            7          /* Radio/TV - Priorite BASSE        */

/*===========================================================================
 * MPPT (Maximum Power Point Tracking)
 *=========================================================================*/
#define MPPT_DUTY_MIN        10         /* Duty cycle minimum PWM (%)       */
#define MPPT_DUTY_MAX        95         /* Duty cycle maximum PWM (%)       */
#define MPPT_DUTY_INIT       50         /* Duty cycle initial PWM (%)       */
#define MPPT_STEP            1          /* Pas d'ajustement (%)             */

/*===========================================================================
 * LOGGER / HISTORIQUE
 *=========================================================================*/
#define LOG_BUFFER_SIZE      128        /* Nombre d'entrees dans l'histo    */

/*===========================================================================
 * UART
 *=========================================================================*/
#define UART_BAUD_RATE       115200
#define UART_TX_BUF_SIZE     128

/*===========================================================================
 * REGISTRES HARDWARE (adresses memoire Cortex-M0)
 *=========================================================================*/
#define SYSTICK_CSR          (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_RVR          (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_CVR          (*(volatile uint32_t *)0xE000E018)
#define SCB_ICSR             (*(volatile uint32_t *)0xE000ED04)

#endif /* CONFIG_H */
