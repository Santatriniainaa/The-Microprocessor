/*******************************************************************************
 * @file    energy.h
 * @brief   TSAKITSAKY ENERGY - Calculs energetiques et SOC batterie
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef ENERGY_H
#define ENERGY_H

#include <stdint.h>
#include "adc.h"

typedef struct {
    uint32_t v_panel_mv;       /* tension panneau (mV)                     */
    uint32_t v_battery_mv;     /* tension batterie (mV)                    */
    uint32_t i_panel_ma;       /* courant panneau (mA)                     */
    uint32_t i_load_ma;        /* courant total charges (mA)               */
    uint32_t p_solar_mw;       /* puissance solaire (mW)                   */
    uint32_t p_load_mw;        /* puissance consommee (mW)                 */
    int32_t  p_net_mw;         /* bilan net (+ charge, - decharge)         */
    uint32_t soc_percent;      /* etat de charge batterie (0-100%)         */
    uint32_t energy_today_wh;  /* energie produite cumul (Wh)              */
    uint32_t is_charging;      /* 1 si batterie en charge, 0 sinon         */
} energy_data_t;

void     energy_init(energy_data_t *e);
void     energy_update(energy_data_t *e, const adc_context_t *adc);
uint32_t energy_calc_soc(uint32_t v_bat_mv);

#endif /* ENERGY_H */
