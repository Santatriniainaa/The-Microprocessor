/*******************************************************************************
 * @file    energy.c
 * @brief   TSAKITSAKY ENERGY - Calculs energetiques et SOC
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Convertit les valeurs ADC brutes en grandeurs physiques (V, A, W)
 *          et calcule le SOC (State of Charge) par interpolation lineaire
 *          sur la courbe OCV d'une batterie plomb-acide 12V.
 ******************************************************************************/
#include "energy.h"
#include "config.h"

/*===========================================================================
 * TABLE SOC : Courbe OCV (Open Circuit Voltage) batterie plomb 12V
 * Source : datasheet batterie plomb-acide standard
 *=========================================================================*/
static const uint32_t soc_table_mv[]  = {
    10500, 11310, 11580, 11750, 11900,
    12060, 12200, 12320, 12420, 12500, 12700
};
static const uint32_t soc_table_pct[] = {
    0,     10,    20,    30,    40,
    50,    60,    70,    80,    90,    100
};
#define SOC_TABLE_LEN  11

/*---------------------------------------------------------------------------
 * @brief   Calcule le SOC par interpolation lineaire
 * @param   v_bat_mv  tension batterie en millivolts
 * @return  SOC en pourcentage (0-100)
 *-------------------------------------------------------------------------*/
uint32_t energy_calc_soc(uint32_t v_bat_mv) {
    uint32_t i;

    /* Hors limites */
    if (v_bat_mv <= soc_table_mv[0]) return 0;
    if (v_bat_mv >= soc_table_mv[SOC_TABLE_LEN - 1]) return 100;

    /* Interpolation lineaire entre deux points de la table */
    for (i = 1; i < SOC_TABLE_LEN; i++) {
        if (v_bat_mv <= soc_table_mv[i]) {
            uint32_t dv     = soc_table_mv[i] - soc_table_mv[i - 1];
            uint32_t dp     = soc_table_pct[i] - soc_table_pct[i - 1];
            uint32_t offset = v_bat_mv - soc_table_mv[i - 1];
            return soc_table_pct[i - 1] + (offset * dp) / dv;
        }
    }
    return 100;
}

void energy_init(energy_data_t *e) {
    e->v_panel_mv      = 0;
    e->v_battery_mv    = 0;
    e->i_panel_ma      = 0;
    e->i_load_ma       = 0;
    e->p_solar_mw      = 0;
    e->p_load_mw       = 0;
    e->p_net_mw        = 0;
    e->soc_percent     = 50;
    e->energy_today_wh = 0;
    e->is_charging     = 0;
}

void energy_update(energy_data_t *e, const adc_context_t *adc) {
    /* --- Conversion ADC → unites physiques --- */
    uint32_t raw_vp = adc_get_filtered(adc, ADC_CH_VPANEL);
    uint32_t raw_vb = adc_get_filtered(adc, ADC_CH_VBAT);

    e->v_panel_mv   = (raw_vp * VPANEL_FACTOR) / ADC_RESOLUTION;
    e->v_battery_mv = (raw_vb * VBAT_FACTOR)   / ADC_RESOLUTION;
    e->i_panel_ma   = adc_get_filtered(adc, ADC_CH_IPANEL) * IPANEL_FACTOR_MA;
    e->i_load_ma    = adc_get_filtered(adc, ADC_CH_ILOAD)  * ILOAD_FACTOR_MA;

    /* --- Calcul puissances (P = V * I / 1000 pour mW) --- */
    e->p_solar_mw = (e->v_panel_mv * e->i_panel_ma) / 1000;
    e->p_load_mw  = (e->v_battery_mv * e->i_load_ma) / 1000;
    e->p_net_mw   = (int32_t)e->p_solar_mw - (int32_t)e->p_load_mw;

    /* --- Etat de charge --- */
    e->is_charging = (e->p_net_mw > 0) ? 1 : 0;

    /* --- SOC --- */
    e->soc_percent = energy_calc_soc(e->v_battery_mv);

    /* --- Energie cumulee (integration simplifiee) --- */
    /* mW * (1/CONTROL_LOOP_HZ) secondes / 3600 secondes/heure = Wh */
    e->energy_today_wh += e->p_solar_mw / (3600 * CONTROL_LOOP_HZ);
}
