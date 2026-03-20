# P5 - Data Logger Embarque ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Difficulte** : Intermediaire | **Chapitres** : 5-7, 11, 18
> **Simulation** : Keil MDK Cortex-M0 Simulator

---

## 1. Objectif Business

Developper un **systeme d'acquisition de donnees** (data logger) embarque capable de :
- **Echantillonner** des capteurs a intervalle regulier (SysTick)
- **Stocker** les echantillons dans un **buffer circulaire** en RAM
- **Calculer** des statistiques en temps reel (min, max, moyenne, ecart-type)
- **Exporter** les donnees via UART (format CSV)

**Marche** : $2.5B (data loggers 2025) — agriculture, environnement, industrie, energie.

## 2. Architecture

```
 SysTick (100 Hz)
     │
     ▼
 ┌────────────┐     ┌─────────────┐     ┌───────────────┐
 │ ADC Sample │────→│ Ring Buffer │────→│ Stats Engine  │
 │ (2 canaux) │     │ (512 ech.)  │     │ min/max/avg/σ │
 └────────────┘     └──────┬──────┘     └───────┬───────┘
                           │                    │
                    ┌──────▼──────┐       ┌─────▼──────┐
                    │  Timestamp  │       │ UART Export│
                    │  (SysTick)  │       │  (CSV)     │
                    └─────────────┘       └────────────┘
```

## 3. Code Source Complet

### 3.1 `ring_buffer.h` / `ring_buffer.c` - Buffer circulaire

```c
/*--- ring_buffer.h ---*/
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

#define RING_BUF_SIZE  512    /* doit etre puissance de 2 */

typedef struct {
    int32_t  data[RING_BUF_SIZE];
    uint32_t head;             /* index ecriture */
    uint32_t tail;             /* index lecture */
    uint32_t count;            /* nombre d'elements */
    uint32_t overflow_count;   /* compteur de pertes */
} ring_buffer_t;

void     ring_init(ring_buffer_t *rb);
int      ring_push(ring_buffer_t *rb, int32_t value);   /* 0=ok, -1=full */
int      ring_pop(ring_buffer_t *rb, int32_t *value);    /* 0=ok, -1=empty */
int      ring_peek(const ring_buffer_t *rb, uint32_t index, int32_t *value);
uint32_t ring_count(const ring_buffer_t *rb);
int      ring_is_full(const ring_buffer_t *rb);
int      ring_is_empty(const ring_buffer_t *rb);

#endif
```

```c
/*--- ring_buffer.c ---*/
#include "ring_buffer.h"

void ring_init(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overflow_count = 0;
}

int ring_push(ring_buffer_t *rb, int32_t value) {
    if (rb->count >= RING_BUF_SIZE) {
        /* Buffer plein : ecraser la plus ancienne donnee */
        rb->tail = (rb->tail + 1) & (RING_BUF_SIZE - 1);
        rb->overflow_count++;
        rb->count--;
    }
    rb->data[rb->head] = value;
    rb->head = (rb->head + 1) & (RING_BUF_SIZE - 1);
    rb->count++;
    return 0;
}

int ring_pop(ring_buffer_t *rb, int32_t *value) {
    if (rb->count == 0) return -1;
    *value = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) & (RING_BUF_SIZE - 1);
    rb->count--;
    return 0;
}

int ring_peek(const ring_buffer_t *rb, uint32_t index, int32_t *value) {
    if (index >= rb->count) return -1;
    uint32_t real_idx = (rb->tail + index) & (RING_BUF_SIZE - 1);
    *value = rb->data[real_idx];
    return 0;
}

uint32_t ring_count(const ring_buffer_t *rb) { return rb->count; }
int ring_is_full(const ring_buffer_t *rb)  { return rb->count >= RING_BUF_SIZE; }
int ring_is_empty(const ring_buffer_t *rb) { return rb->count == 0; }
```

### 3.2 `stats.h` / `stats.c` - Moteur de statistiques

```c
/*--- stats.h ---*/
#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include "ring_buffer.h"

typedef struct {
    int32_t  min;
    int32_t  max;
    int32_t  mean;           /* moyenne (entiere) */
    uint32_t variance;       /* variance (entiere) */
    uint32_t std_dev;        /* ecart-type approx (isqrt) */
    uint32_t sample_count;
} stats_result_t;

void stats_compute(const ring_buffer_t *rb, stats_result_t *result);
uint32_t isqrt(uint32_t n);   /* racine carree entiere */

#endif
```

```c
/*--- stats.c ---*/
#include "stats.h"

/* Racine carree entiere (algorithme de Newton) */
uint32_t isqrt(uint32_t n) {
    if (n == 0) return 0;
    uint32_t x = n;
    uint32_t y = (x + 1) / 2;
    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}

void stats_compute(const ring_buffer_t *rb, stats_result_t *result) {
    uint32_t count = ring_count(rb);
    if (count == 0) {
        result->min = 0;
        result->max = 0;
        result->mean = 0;
        result->variance = 0;
        result->std_dev = 0;
        result->sample_count = 0;
        return;
    }

    int32_t val;
    ring_peek(rb, 0, &val);

    int32_t  min_val = val;
    int32_t  max_val = val;
    int64_t  sum     = 0;   /* utiliser 64 bits pour eviter overflow */

    /* Passe 1 : min, max, somme */
    for (uint32_t i = 0; i < count; i++) {
        ring_peek(rb, i, &val);
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
        sum += val;
    }

    int32_t mean = (int32_t)(sum / (int64_t)count);

    /* Passe 2 : variance */
    uint64_t var_sum = 0;
    for (uint32_t i = 0; i < count; i++) {
        ring_peek(rb, i, &val);
        int32_t diff = val - mean;
        var_sum += (uint64_t)((int64_t)diff * diff);
    }

    result->min          = min_val;
    result->max          = max_val;
    result->mean         = mean;
    result->variance     = (uint32_t)(var_sum / count);
    result->std_dev      = isqrt(result->variance);
    result->sample_count = count;
}
```

### 3.3 `datalogger.h` / `datalogger.c` - Module principal

```c
/*--- datalogger.h ---*/
#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <stdint.h>
#include "ring_buffer.h"
#include "stats.h"

#define DL_NUM_CHANNELS  2
#define DL_SAMPLE_RATE   100   /* Hz */

typedef struct {
    ring_buffer_t  channels[DL_NUM_CHANNELS];
    stats_result_t stats[DL_NUM_CHANNELS];
    uint32_t       sample_count;
    uint32_t       timestamp_ms;
    uint32_t       is_recording;
} datalogger_t;

void dl_init(datalogger_t *dl);
void dl_start(datalogger_t *dl);
void dl_stop(datalogger_t *dl);
void dl_sample(datalogger_t *dl);     /* appele par SysTick ISR */
void dl_compute_stats(datalogger_t *dl);
void dl_export_csv(const datalogger_t *dl);
void dl_print_stats(const datalogger_t *dl);

#endif
```

```c
/*--- datalogger.c ---*/
#include "datalogger.h"
#include "uart_io.h"

/* Simulation de 2 canaux ADC */
static int32_t simulate_adc_ch0(uint32_t t) {
    /* Signal sinusoidal simule : temperature 20-30°C */
    static const int32_t sine_lut[] = {
        25, 26, 27, 28, 29, 30, 30, 29, 28, 27,
        26, 25, 24, 23, 22, 21, 20, 20, 21, 22,
        23, 24
    };
    return sine_lut[t % 22] * 100;  /* en centieme de degre */
}

static int32_t simulate_adc_ch1(uint32_t t) {
    /* Signal : pression 1000-1020 hPa avec bruit */
    int32_t base = 101000;  /* 1010.00 hPa en centieme */
    int32_t noise = ((t * 7 + 13) % 200) - 100;  /* ±1 hPa */
    return base + noise;
}

void dl_init(datalogger_t *dl) {
    for (uint32_t i = 0; i < DL_NUM_CHANNELS; i++) {
        ring_init(&dl->channels[i]);
    }
    dl->sample_count = 0;
    dl->timestamp_ms = 0;
    dl->is_recording = 0;
}

void dl_start(datalogger_t *dl) {
    dl->is_recording = 1;
    uart_send_string("[DL] Enregistrement demarre\r\n");
}

void dl_stop(datalogger_t *dl) {
    dl->is_recording = 0;
    uart_send_string("[DL] Enregistrement arrete\r\n");
}

void dl_sample(datalogger_t *dl) {
    if (!dl->is_recording) return;

    int32_t ch0 = simulate_adc_ch0(dl->sample_count);
    int32_t ch1 = simulate_adc_ch1(dl->sample_count);

    ring_push(&dl->channels[0], ch0);
    ring_push(&dl->channels[1], ch1);

    dl->sample_count++;
    dl->timestamp_ms += (1000 / DL_SAMPLE_RATE);
}

void dl_compute_stats(datalogger_t *dl) {
    for (uint32_t i = 0; i < DL_NUM_CHANNELS; i++) {
        stats_compute(&dl->channels[i], &dl->stats[i]);
    }
}

void dl_print_stats(const datalogger_t *dl) {
    static const char *ch_names[] = {"Temperature", "Pression   "};

    uart_send_string("\r\n======= STATISTIQUES =======\r\n");
    uart_send_string("Echantillons: ");
    uart_send_int(dl->sample_count);
    uart_send_string("  Duree: ");
    uart_send_int(dl->timestamp_ms / 1000);
    uart_send_string("s\r\n\r\n");

    for (uint32_t i = 0; i < DL_NUM_CHANNELS; i++) {
        const stats_result_t *s = &dl->stats[i];
        uart_send_string(ch_names[i]);
        uart_send_string(": Min=");
        uart_send_int(s->min);
        uart_send_string(" Max=");
        uart_send_int(s->max);
        uart_send_string(" Moy=");
        uart_send_int(s->mean);
        uart_send_string(" Sigma=");
        uart_send_int(s->std_dev);
        uart_send_string(" (N=");
        uart_send_int(s->sample_count);
        uart_send_string(")\r\n");
    }

    uart_send_string("Overflow CH0: ");
    uart_send_int(dl->channels[0].overflow_count);
    uart_send_string("  CH1: ");
    uart_send_int(dl->channels[1].overflow_count);
    uart_send_string("\r\n============================\r\n");
}

void dl_export_csv(const datalogger_t *dl) {
    uart_send_string("timestamp_ms,ch0_temp,ch1_pressure\r\n");

    uint32_t count = ring_count(&dl->channels[0]);
    uint32_t ts = 0;

    for (uint32_t i = 0; i < count; i++) {
        int32_t v0, v1;
        ring_peek(&dl->channels[0], i, &v0);
        ring_peek(&dl->channels[1], i, &v1);

        uart_send_int(ts);
        uart_send_char(',');
        uart_send_int(v0);
        uart_send_char(',');
        uart_send_int(v1);
        uart_send_string("\r\n");

        ts += (1000 / DL_SAMPLE_RATE);
    }
}
```

### 3.4 `main.c`

```c
/*******************************************************************************
 * @file    main.c
 * @brief   Data Logger - Programme principal avec menu UART
 ******************************************************************************/
#include "datalogger.h"
#include "uart_io.h"

static datalogger_t logger;
static volatile uint32_t tick_flag = 0;

void SysTick_Handler(void) {
    tick_flag = 1;
}

static void print_menu(void) {
    uart_send_string("\r\n--- Data Logger ARM Cortex-M0 ---\r\n");
    uart_send_string("  s = Start enregistrement\r\n");
    uart_send_string("  p = Pause enregistrement\r\n");
    uart_send_string("  t = Afficher statistiques\r\n");
    uart_send_string("  e = Exporter CSV\r\n");
    uart_send_string("  r = Reset\r\n");
    uart_send_string("  h = Aide\r\n");
    uart_send_string("--------------------------------\r\n");
}

int main(void) {
    uart_init(115200);
    dl_init(&logger);

    /* SysTick : 10 ms (100 Hz) a 12 MHz */
    *(volatile uint32_t *)0xE000E014 = 120000 - 1;
    *(volatile uint32_t *)0xE000E018 = 0;
    *(volatile uint32_t *)0xE000E010 = 0x07;

    print_menu();
    uart_send_string("\r\n> ");

    while (1) {
        /* Echantillonnage periodique */
        if (tick_flag) {
            tick_flag = 0;
            dl_sample(&logger);
        }

        /* Commandes utilisateur (non bloquant simplifie) */
        if (!(*(volatile uint32_t *)0x40004018 & (1 << 4))) {
            char cmd = (char)(*(volatile uint32_t *)0x40004000 & 0xFF);

            switch (cmd) {
            case 's': dl_start(&logger);         break;
            case 'p': dl_stop(&logger);          break;
            case 't':
                dl_compute_stats(&logger);
                dl_print_stats(&logger);
                break;
            case 'e':
                dl_stop(&logger);
                dl_export_csv(&logger);
                break;
            case 'r':
                dl_stop(&logger);
                dl_init(&logger);
                uart_send_string("[DL] Reset effectue\r\n");
                break;
            case 'h': print_menu(); break;
            }
            uart_send_string("\r\n> ");
        }
    }
}
```

### 3.5 Sortie attendue (commande `t`)

```
======= STATISTIQUES =======
Echantillons: 1000  Duree: 10s

Temperature : Min=2000 Max=3000 Moy=2500 Sigma=316 (N=512)
Pression    : Min=100900 Max=101100 Moy=101000 Sigma=57 (N=512)
Overflow CH0: 488  CH1: 488
============================
```

---

[Retour a l'index](../00-INDEX-PROJET.md)
