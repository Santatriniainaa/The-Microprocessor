/*******************************************************************************
 * @file    adc.c
 * @brief   TSAKITSAKY ENERGY - Implementation acquisition ADC
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Mode SIMULATION : genere des donnees realistes representant une
 *          journee type a Madagascar (ensoleillement 06h-18h, pic a 12h).
 *          En production, remplacer simulate_channel() par les vrais
 *          registres ADC du microcontroleur.
 ******************************************************************************/
#include "adc.h"

/*===========================================================================
 * SIMULATION : Donnees realistes pour le simulateur Keil
 * En production, supprimer cette section et lire les registres ADC reels
 *=========================================================================*/

static uint32_t sim_tick = 0;

/* Courbe solaire journee type Madagascar (index = heure 0-23)
 * Valeurs en unites arbitraires proportionnelles a l'irradiance
 * Pic a midi ~3600, lever ~06h, coucher ~18h                              */
static const uint16_t solar_curve[24] = {
    0, 0, 0, 0, 0, 50,            /* 00h-05h : nuit                       */
    200, 600, 1200, 2000, 2800,    /* 06h-10h : matin                      */
    3200, 3500, 3600, 3500, 3200,  /* 11h-15h : pic solaire                */
    2800, 2000, 1200, 600,         /* 16h-19h : apres-midi                 */
    200, 50, 0, 0                  /* 20h-23h : nuit                       */
};

/* Tension batterie simulee (evolue selon production/consommation) */
static int32_t bat_sim_mv = 12200;

static uint32_t simulate_channel(uint32_t ch) {
    /* 1 heure simulee = 360 ticks a 10 Hz (36 secondes reelles) */
    uint32_t hour = (sim_tick / 360) % 24;
    /* Bruit pseudo-aleatoire simple (deterministe, reproductible) */
    uint32_t noise = ((sim_tick * 7 + ch * 13 + 37) % 50);

    switch (ch) {
    case ADC_CH_VPANEL: {
        uint32_t solar = solar_curve[hour];
        /* Vpanel entre 10V (nuit, circuit ouvert bas) et 28V (plein soleil) */
        uint32_t vpanel_mv = 10000 + (solar * 5);
        return (vpanel_mv * ADC_RESOLUTION) / VPANEL_FACTOR;
    }
    case ADC_CH_VBAT: {
        /* Batterie : augmente avec soleil, diminue la nuit */
        uint32_t solar = solar_curve[hour];
        if (solar > 500) {
            if (bat_sim_mv < 12700) bat_sim_mv += 2;  /* charge lente */
        } else {
            if (bat_sim_mv > 10500) bat_sim_mv -= 3;  /* decharge */
        }
        return ((uint32_t)bat_sim_mv * ADC_RESOLUTION) / VBAT_FACTOR + noise;
    }
    case ADC_CH_IPANEL: {
        uint32_t solar = solar_curve[hour];
        return (solar > 0) ? (solar + noise) : 0;
    }
    case ADC_CH_ILOAD:
        /* Consommation moyenne ~1.5A avec variations */
        return 1500 + noise;
    default:
        return 0;
    }
}

/*===========================================================================
 * API PUBLIQUE
 *=========================================================================*/

void adc_init(adc_context_t *ctx) {
    uint32_t ch, j;
    for (ch = 0; ch < ADC_NUM_CHANNELS; ch++) {
        ctx->raw[ch] = 0;
        ctx->filtered[ch] = 0;
        for (j = 0; j < ADC_FILTER_SIZE; j++) {
            ctx->filter_buf[ch][j] = 0;
        }
    }
    ctx->filter_idx = 0;
    ctx->sample_count = 0;
    sim_tick = 0;
    bat_sim_mv = 12200;
}

void adc_sample(adc_context_t *ctx) {
    uint32_t idx = ctx->filter_idx & (ADC_FILTER_SIZE - 1);
    uint32_t ch, j, sum;

    for (ch = 0; ch < ADC_NUM_CHANNELS; ch++) {
        /* Lire le canal ADC (simulation ou reel) */
        ctx->raw[ch] = simulate_channel(ch);

        /* Stocker dans le buffer de moyennage */
        ctx->filter_buf[ch][idx] = ctx->raw[ch];

        /* Moyennage glissant sur ADC_FILTER_SIZE echantillons */
        sum = 0;
        for (j = 0; j < ADC_FILTER_SIZE; j++) {
            sum += ctx->filter_buf[ch][j];
        }
        ctx->filtered[ch] = sum / ADC_FILTER_SIZE;
    }

    ctx->filter_idx++;
    ctx->sample_count++;
    sim_tick++;
}

uint32_t adc_get_raw(const adc_context_t *ctx, uint32_t channel) {
    if (channel >= ADC_NUM_CHANNELS) return 0;
    return ctx->raw[channel];
}

uint32_t adc_get_filtered(const adc_context_t *ctx, uint32_t channel) {
    if (channel >= ADC_NUM_CHANNELS) return 0;
    return ctx->filtered[channel];
}
