/*******************************************************************************
 * @file    adc.h
 * @brief   TSAKITSAKY ENERGY - Module acquisition ADC multi-canaux
 * @author  Santatriniaina RAZAFINDRAKOTO (Firmware Lead)
 * @date    20 mars 2026
 *
 * @details Gere l'echantillonnage de 4 canaux ADC avec moyennage glissant
 *          sur 8 points pour le filtrage du bruit.
 *          En mode simulation, genere des donnees realistes de panneau
 *          solaire et batterie suivant une courbe journaliere.
 ******************************************************************************/
#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "config.h"

/*--- Structure de contexte ADC ---*/
typedef struct {
    uint32_t raw[ADC_NUM_CHANNELS];                          /* brut         */
    uint32_t filtered[ADC_NUM_CHANNELS];                     /* filtre       */
    uint32_t filter_buf[ADC_NUM_CHANNELS][ADC_FILTER_SIZE];  /* ring buffer  */
    uint32_t filter_idx;                                     /* index courant*/
    uint32_t sample_count;                                   /* total echan. */
} adc_context_t;

/*--- API publique ---*/
void     adc_init(adc_context_t *ctx);
void     adc_sample(adc_context_t *ctx);
uint32_t adc_get_raw(const adc_context_t *ctx, uint32_t channel);
uint32_t adc_get_filtered(const adc_context_t *ctx, uint32_t channel);

#endif /* ADC_H */
