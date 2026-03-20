# 05 - Code Source Complet : TSAKITSAKY ENERGY

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. `config.h` - Configuration Systeme

```c
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/*=== Parametres systeme ===*/
#define SYS_CLOCK_HZ        12000000   /* 12 MHz (simulateur Keil) */
#define TICK_HZ              1000       /* SysTick a 1 kHz */
#define CONTROL_LOOP_HZ      10         /* Boucle principale 10 Hz */
#define DISPLAY_HZ            1         /* Affichage 1 Hz */
#define LOG_INTERVAL_S       60         /* Log toutes les 60 s */

/*=== Seuils batterie (millivolts) ===*/
#define BAT_FULL_MV          12700      /* 12.70V = 100% */
#define BAT_NORMAL_MV        12200      /* 12.20V = 60% */
#define BAT_LOW_MV           11750      /* 11.75V = 30% */
#define BAT_CRITICAL_MV      11310      /* 11.31V = 10% */
#define BAT_SHUTDOWN_MV      10500      /* 10.50V = 0% */
#define BAT_OVERCHARGE_MV    14400      /* 14.4V seuil surcharge */

/*=== Seuils courant (milliamperes) ===*/
#define MAX_LOAD_CURRENT_MA  8000       /* 8A max total */
#define MAX_SINGLE_LOAD_MA   5000       /* 5A max par charge */

/*=== Seuils temperature (degres C x10) ===*/
#define TEMP_WARNING          550       /* 55.0°C */
#define TEMP_SHUTDOWN          650       /* 65.0°C */

/*=== ADC ===*/
#define ADC_RESOLUTION       4096       /* 12 bits */
#define ADC_VREF_MV          3300       /* 3.3V reference */

/* Facteurs de conversion ADC → tension reelle (x1000) */
#define VPANEL_FACTOR        7800       /* V_real = adc * 7800 / 4096 */
#define VBAT_FACTOR          4900       /* V_real = adc * 4900 / 4096 */

/*=== Charges ===*/
#define NUM_LOADS             3
#define LOAD1_PIN             5         /* GPIO bit pour charge 1 */
#define LOAD2_PIN             6
#define LOAD3_PIN             7

/*=== Logger ===*/
#define LOG_BUFFER_SIZE       128       /* 128 entrees de log */

#endif /* CONFIG_H */
```

## 2. `adc.h` / `adc.c` - Acquisition Multi-canaux

```c
/*--- adc.h ---*/
#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_NUM_CHANNELS  4

/* Canaux */
#define ADC_CH_VPANEL   0
#define ADC_CH_VBAT     1
#define ADC_CH_IPANEL   2
#define ADC_CH_ILOAD    3

typedef struct {
    uint32_t raw[ADC_NUM_CHANNELS];        /* valeurs brutes ADC */
    uint32_t filtered[ADC_NUM_CHANNELS];   /* apres moyennage */
    uint32_t filter_buf[ADC_NUM_CHANNELS][8]; /* buffer moyennage 8 pts */
    uint32_t filter_idx;
} adc_context_t;

void     adc_init(adc_context_t *ctx);
void     adc_sample(adc_context_t *ctx);    /* echantillonner tous les canaux */
uint32_t adc_get(const adc_context_t *ctx, uint32_t channel);

#endif
```

```c
/*--- adc.c ---*/
#include "adc.h"
#include "config.h"

/* Simulation ADC : sequences de donnees realistes */
static uint32_t sim_tick = 0;

/* Simulation panneau solaire : courbe journee (index = heure simulee) */
static const uint16_t solar_curve[] = {
    0, 0, 0, 50, 200, 600, 1200, 2000, 2800, 3200,
    3500, 3600, 3500, 3200, 2800, 2000, 1200, 600, 200, 50,
    0, 0, 0, 0
};

/* Simulation batterie : tension qui evolue */
static uint16_t bat_sim_mv = 12200;

static uint32_t simulate_channel(uint32_t ch) {
    uint32_t hour = (sim_tick / 360) % 24; /* 1 heure = 360 ticks (a 10 Hz) */
    uint32_t noise = ((sim_tick * 7 + ch * 13) % 50);

    switch (ch) {
    case ADC_CH_VPANEL: {
        /* Tension panneau proportionnelle au soleil */
        uint32_t solar = solar_curve[hour];
        uint32_t vpanel_mv = 10000 + (solar * 5); /* 10-28V */
        return (vpanel_mv * ADC_RESOLUTION) / VPANEL_FACTOR;
    }
    case ADC_CH_VBAT:
        return (bat_sim_mv * ADC_RESOLUTION) / VBAT_FACTOR + noise;
    case ADC_CH_IPANEL: {
        uint32_t solar = solar_curve[hour];
        return solar + noise;  /* courant proportionnel au soleil */
    }
    case ADC_CH_ILOAD:
        return 1500 + noise;   /* ~1.5A consommation moyenne */
    default:
        return 0;
    }
}

void adc_init(adc_context_t *ctx) {
    for (uint32_t ch = 0; ch < ADC_NUM_CHANNELS; ch++) {
        ctx->raw[ch] = 0;
        ctx->filtered[ch] = 0;
        for (uint32_t j = 0; j < 8; j++) {
            ctx->filter_buf[ch][j] = 0;
        }
    }
    ctx->filter_idx = 0;
    sim_tick = 0;
}

void adc_sample(adc_context_t *ctx) {
    uint32_t idx = ctx->filter_idx & 7;

    for (uint32_t ch = 0; ch < ADC_NUM_CHANNELS; ch++) {
        /* Lire ADC (simulation) */
        ctx->raw[ch] = simulate_channel(ch);
        ctx->filter_buf[ch][idx] = ctx->raw[ch];

        /* Moyennage glissant sur 8 echantillons */
        uint32_t sum = 0;
        for (uint32_t j = 0; j < 8; j++) {
            sum += ctx->filter_buf[ch][j];
        }
        ctx->filtered[ch] = sum >> 3; /* diviser par 8 */
    }
    ctx->filter_idx++;
    sim_tick++;
}

uint32_t adc_get(const adc_context_t *ctx, uint32_t channel) {
    if (channel >= ADC_NUM_CHANNELS) return 0;
    return ctx->filtered[channel];
}
```

## 3. `energy.h` / `energy.c` - Calculs Energetiques

```c
/*--- energy.h ---*/
#ifndef ENERGY_H
#define ENERGY_H

#include <stdint.h>
#include "adc.h"

typedef struct {
    uint32_t v_panel_mv;      /* tension panneau (mV) */
    uint32_t v_battery_mv;    /* tension batterie (mV) */
    uint32_t i_panel_ma;      /* courant panneau (mA) */
    uint32_t i_load_ma;       /* courant charges (mA) */
    uint32_t p_solar_mw;      /* puissance solaire (mW) */
    uint32_t p_load_mw;       /* puissance consommee (mW) */
    int32_t  p_net_mw;        /* bilan net (+ = charge, - = decharge) */
    uint32_t soc_percent;     /* etat de charge batterie (0-100%) */
    uint32_t energy_today_wh; /* energie produite aujourd'hui (Wh) */
} energy_data_t;

void     energy_init(energy_data_t *e);
void     energy_update(energy_data_t *e, const adc_context_t *adc);
uint32_t energy_calc_soc(uint32_t v_bat_mv);

#endif
```

```c
/*--- energy.c ---*/
#include "energy.h"
#include "config.h"

/* Table SOC : tension (mV) → pourcentage */
static const uint32_t soc_table_mv[]  = {10500,11310,11580,11750,11900,12060,12200,12320,12420,12500,12700};
static const uint32_t soc_table_pct[] = {    0,   10,   20,   30,   40,   50,   60,   70,   80,   90,  100};
#define SOC_TABLE_LEN 11

uint32_t energy_calc_soc(uint32_t v_bat_mv) {
    if (v_bat_mv <= soc_table_mv[0]) return 0;
    if (v_bat_mv >= soc_table_mv[SOC_TABLE_LEN - 1]) return 100;

    for (uint32_t i = 1; i < SOC_TABLE_LEN; i++) {
        if (v_bat_mv <= soc_table_mv[i]) {
            /* Interpolation lineaire */
            uint32_t dv = soc_table_mv[i] - soc_table_mv[i-1];
            uint32_t dp = soc_table_pct[i] - soc_table_pct[i-1];
            uint32_t offset = v_bat_mv - soc_table_mv[i-1];
            return soc_table_pct[i-1] + (offset * dp) / dv;
        }
    }
    return 100;
}

void energy_init(energy_data_t *e) {
    e->v_panel_mv     = 0;
    e->v_battery_mv   = 0;
    e->i_panel_ma     = 0;
    e->i_load_ma      = 0;
    e->p_solar_mw     = 0;
    e->p_load_mw      = 0;
    e->p_net_mw       = 0;
    e->soc_percent    = 50;
    e->energy_today_wh = 0;
}

void energy_update(energy_data_t *e, const adc_context_t *adc) {
    /* Convertir ADC → unites reelles */
    uint32_t raw_vp = adc_get(adc, ADC_CH_VPANEL);
    uint32_t raw_vb = adc_get(adc, ADC_CH_VBAT);

    e->v_panel_mv   = (raw_vp * VPANEL_FACTOR) / ADC_RESOLUTION;
    e->v_battery_mv = (raw_vb * VBAT_FACTOR) / ADC_RESOLUTION;
    e->i_panel_ma   = adc_get(adc, ADC_CH_IPANEL);
    e->i_load_ma    = adc_get(adc, ADC_CH_ILOAD);

    /* Puissances */
    e->p_solar_mw = (e->v_panel_mv * e->i_panel_ma) / 1000;
    e->p_load_mw  = (e->v_battery_mv * e->i_load_ma) / 1000;
    e->p_net_mw   = (int32_t)e->p_solar_mw - (int32_t)e->p_load_mw;

    /* SOC */
    e->soc_percent = energy_calc_soc(e->v_battery_mv);

    /* Energie cumulee (simplifiee) */
    e->energy_today_wh += e->p_solar_mw / (3600 * CONTROL_LOOP_HZ);
}
```

## 4. `load.h` / `load.c` - Controle des Charges

```c
/*--- load.h ---*/
#ifndef LOAD_H
#define LOAD_H

#include <stdint.h>
#include "config.h"

typedef struct {
    uint32_t enabled[NUM_LOADS];    /* 0=OFF, 1=ON */
    uint32_t priority[NUM_LOADS];   /* 0=haute, 2=basse */
    uint32_t gpio_reg;              /* registre GPIO simule */
} load_context_t;

void load_init(load_context_t *lc);
void load_set(load_context_t *lc, uint32_t load_id, uint32_t on);
void load_set_all(load_context_t *lc, uint32_t l0, uint32_t l1, uint32_t l2);
void load_apply(load_context_t *lc);

#endif
```

```c
/*--- load.c ---*/
#include "load.h"

volatile uint32_t LOAD_GPIO_REG = 0;  /* observable dans Memory Window */

void load_init(load_context_t *lc) {
    lc->priority[0] = 0;  /* eclairage = haute priorite */
    lc->priority[1] = 1;  /* chargeur  = moyenne */
    lc->priority[2] = 2;  /* radio/TV  = basse */
    for (uint32_t i = 0; i < NUM_LOADS; i++) lc->enabled[i] = 0;
    lc->gpio_reg = 0;
    LOAD_GPIO_REG = 0;
}

void load_set(load_context_t *lc, uint32_t id, uint32_t on) {
    if (id < NUM_LOADS) lc->enabled[id] = on ? 1 : 0;
}

void load_set_all(load_context_t *lc, uint32_t l0, uint32_t l1, uint32_t l2) {
    lc->enabled[0] = l0 ? 1 : 0;
    lc->enabled[1] = l1 ? 1 : 0;
    lc->enabled[2] = l2 ? 1 : 0;
}

void load_apply(load_context_t *lc) {
    uint32_t reg = 0;
    if (lc->enabled[0]) reg |= (1 << LOAD1_PIN);
    if (lc->enabled[1]) reg |= (1 << LOAD2_PIN);
    if (lc->enabled[2]) reg |= (1 << LOAD3_PIN);
    lc->gpio_reg = reg;
    LOAD_GPIO_REG = reg;
}
```

## 5. `fsm.h` / `fsm.c` - Machine a Etats Principale

```c
/*--- fsm.h ---*/
#ifndef FSM_H
#define FSM_H

#include "energy.h"
#include "load.h"
#include "alert.h"

typedef enum {
    FSM_STARTUP  = 0,
    FSM_NORMAL   = 1,
    FSM_LOW_BAT  = 2,
    FSM_CRITICAL = 3,
    FSM_SHUTDOWN = 4,
    FSM_FAULT    = 5
} fsm_state_t;

typedef struct {
    fsm_state_t state;
    fsm_state_t prev_state;
    uint32_t    state_timer;     /* ticks dans l'etat courant */
    uint32_t    uptime_s;
} fsm_context_t;

void fsm_init(fsm_context_t *fsm);
void fsm_update(fsm_context_t *fsm, const energy_data_t *energy,
                load_context_t *loads, alert_context_t *alerts);
const char *fsm_state_name(fsm_state_t s);

#endif
```

```c
/*--- fsm.c ---*/
#include "fsm.h"
#include "config.h"

static const char *state_names[] = {
    "STARTUP", "NORMAL", "LOW_BAT", "CRITICAL", "SHUTDOWN", "FAULT"
};

const char *fsm_state_name(fsm_state_t s) {
    return (s <= FSM_FAULT) ? state_names[s] : "UNKNOWN";
}

void fsm_init(fsm_context_t *fsm) {
    fsm->state       = FSM_STARTUP;
    fsm->prev_state  = FSM_STARTUP;
    fsm->state_timer = 0;
    fsm->uptime_s    = 0;
}

void fsm_update(fsm_context_t *fsm, const energy_data_t *energy,
                load_context_t *loads, alert_context_t *alerts) {

    fsm->state_timer++;
    fsm_state_t next = fsm->state;
    uint32_t soc = energy->soc_percent;
    uint32_t vbat = energy->v_battery_mv;

    switch (fsm->state) {

    case FSM_STARTUP:
        load_set_all(loads, 0, 0, 0);
        alert_set_led(alerts, LED_GREEN, 1);
        if (fsm->state_timer > 20) {  /* 2 secondes d'init */
            if (vbat > BAT_SHUTDOWN_MV) {
                next = (soc > 30) ? FSM_NORMAL : FSM_LOW_BAT;
            } else {
                next = FSM_SHUTDOWN;
            }
        }
        break;

    case FSM_NORMAL:
        load_set_all(loads, 1, 1, 1);  /* toutes charges ON */
        alert_set_led(alerts, LED_GREEN, 1);
        alert_set_led(alerts, LED_ORANGE, 0);
        alert_set_led(alerts, LED_RED, 0);

        if (vbat > BAT_OVERCHARGE_MV) {
            /* Surcharge : reduire le courant de charge */
            alert_set_led(alerts, LED_ORANGE, 1);
        }
        if (soc < 30) next = FSM_LOW_BAT;
        if (soc < 10) next = FSM_CRITICAL;
        break;

    case FSM_LOW_BAT:
        load_set_all(loads, 1, 1, 0);  /* couper charge basse priorite */
        alert_set_led(alerts, LED_GREEN, 0);
        alert_set_led(alerts, LED_ORANGE, 1);
        alert_set_led(alerts, LED_RED, 0);

        if (soc > 40) next = FSM_NORMAL;   /* hysteresis +10% */
        if (soc < 10) next = FSM_CRITICAL;
        break;

    case FSM_CRITICAL:
        load_set_all(loads, 1, 0, 0);  /* eclairage seulement */
        alert_set_led(alerts, LED_GREEN, 0);
        alert_set_led(alerts, LED_ORANGE, 0);
        alert_set_led(alerts, LED_RED, 1);

        if (soc > 20) next = FSM_LOW_BAT;  /* hysteresis */
        if (vbat < BAT_SHUTDOWN_MV) next = FSM_SHUTDOWN;
        break;

    case FSM_SHUTDOWN:
        load_set_all(loads, 0, 0, 0);  /* tout OFF */
        alert_set_led(alerts, LED_GREEN, 0);
        alert_set_led(alerts, LED_ORANGE, 0);
        alert_set_led(alerts, LED_RED, (fsm->state_timer >> 3) & 1); /* clignotant */

        /* Reveil si panneau fournit du courant et batterie remonte */
        if (energy->p_solar_mw > 1000 && vbat > BAT_CRITICAL_MV) {
            next = FSM_CRITICAL;
        }
        break;

    case FSM_FAULT:
        load_set_all(loads, 0, 0, 0);
        alert_set_led(alerts, LED_RED, 1);
        break;
    }

    if (next != fsm->state) {
        fsm->prev_state = fsm->state;
        fsm->state = next;
        fsm->state_timer = 0;
    }

    load_apply(loads);
    alert_apply(alerts);
}
```

## 6. `alert.h` / `alert.c` - Alertes et LEDs

```c
/*--- alert.h ---*/
#ifndef ALERT_H
#define ALERT_H
#include <stdint.h>

#define LED_GREEN   0
#define LED_ORANGE  1
#define LED_RED     2
#define NUM_LEDS    3

typedef struct {
    uint32_t led_state[NUM_LEDS];
    uint32_t gpio_led_reg;
} alert_context_t;

void alert_init(alert_context_t *a);
void alert_set_led(alert_context_t *a, uint32_t led, uint32_t on);
void alert_apply(alert_context_t *a);

#endif
```

```c
/*--- alert.c ---*/
#include "alert.h"

volatile uint32_t ALERT_LED_REG = 0;

void alert_init(alert_context_t *a) {
    for (uint32_t i = 0; i < NUM_LEDS; i++) a->led_state[i] = 0;
    a->gpio_led_reg = 0;
    ALERT_LED_REG = 0;
}

void alert_set_led(alert_context_t *a, uint32_t led, uint32_t on) {
    if (led < NUM_LEDS) a->led_state[led] = on ? 1 : 0;
}

void alert_apply(alert_context_t *a) {
    uint32_t reg = 0;
    if (a->led_state[LED_GREEN])  reg |= (1 << 0);
    if (a->led_state[LED_ORANGE]) reg |= (1 << 1);
    if (a->led_state[LED_RED])    reg |= (1 << 2);
    a->gpio_led_reg = reg;
    ALERT_LED_REG = reg;
}
```

## 7. `display.c` - Affichage UART

```c
/*--- display.c ---*/
#include "display.h"
#include "uart_io.h"
#include "fsm.h"
#include "energy.h"
#include "load.h"

void display_status(const fsm_context_t *fsm, const energy_data_t *e,
                    const load_context_t *loads) {
    uart_send_string("[");
    uart_send_int(fsm->uptime_s);
    uart_send_string("s] ");
    uart_send_string(fsm_state_name(fsm->state));

    uart_send_string(" | Bat:");
    uart_send_int(e->v_battery_mv / 100);
    uart_send_char('.');
    uart_send_int((e->v_battery_mv / 10) % 10);
    uart_send_string("V ");
    uart_send_int(e->soc_percent);
    uart_send_string("%");

    uart_send_string(" | Sol:");
    uart_send_int(e->p_solar_mw / 1000);
    uart_send_char('.');
    uart_send_int((e->p_solar_mw / 100) % 10);
    uart_send_string("W");

    uart_send_string(" | Load:");
    uart_send_int(e->p_load_mw / 1000);
    uart_send_char('.');
    uart_send_int((e->p_load_mw / 100) % 10);
    uart_send_string("W");

    uart_send_string(" | L:");
    uart_send_char(loads->enabled[0] ? '1' : '0');
    uart_send_char(loads->enabled[1] ? '1' : '0');
    uart_send_char(loads->enabled[2] ? '1' : '0');

    uart_send_string("\r\n");
}

void display_banner(void) {
    uart_send_string("\r\n");
    uart_send_string("╔═══════════════════════════════════════╗\r\n");
    uart_send_string("║   TSAKITSAKY ENERGY v1.0              ║\r\n");
    uart_send_string("║   Controleur Solaire Intelligent      ║\r\n");
    uart_send_string("║   ARM Cortex-M0                       ║\r\n");
    uart_send_string("║   (c) 2026 S. RAZAFINDRAKOTO          ║\r\n");
    uart_send_string("╚═══════════════════════════════════════╝\r\n\r\n");
}
```

## 8. `main.c` - Programme Principal

```c
/*******************************************************************************
 * @file    main.c
 * @brief   TSAKITSAKY ENERGY - Programme principal
 * @author  Santatriniaina RAZAFINDRAKOTO
 * @date    20 mars 2026
 ******************************************************************************/
#include "config.h"
#include "adc.h"
#include "energy.h"
#include "load.h"
#include "alert.h"
#include "fsm.h"
#include "display.h"
#include "uart_io.h"

/*--- Contextes globaux ---*/
static adc_context_t    g_adc;
static energy_data_t    g_energy;
static load_context_t   g_loads;
static alert_context_t  g_alerts;
static fsm_context_t    g_fsm;

/*--- Drapeaux temporels (SysTick) ---*/
static volatile uint32_t g_tick_ms = 0;
static volatile uint32_t g_flag_10hz = 0;
static volatile uint32_t g_flag_1hz = 0;

void SysTick_Handler(void) {
    g_tick_ms++;
    if ((g_tick_ms % (TICK_HZ / CONTROL_LOOP_HZ)) == 0) g_flag_10hz = 1;
    if ((g_tick_ms % TICK_HZ) == 0) g_flag_1hz = 1;
}

int main(void) {
    /* Initialisation */
    uart_init(115200);
    adc_init(&g_adc);
    energy_init(&g_energy);
    load_init(&g_loads);
    alert_init(&g_alerts);
    fsm_init(&g_fsm);

    display_banner();
    uart_send_string("Initialisation...\r\n");

    /* Configurer SysTick : 1 ms */
    *(volatile uint32_t *)0xE000E014 = (SYS_CLOCK_HZ / TICK_HZ) - 1;
    *(volatile uint32_t *)0xE000E018 = 0;
    *(volatile uint32_t *)0xE000E010 = 0x07;

    uart_send_string("Systeme pret.\r\n\r\n");

    /* === BOUCLE PRINCIPALE === */
    while (1) {

        /* Boucle de controle a 10 Hz */
        if (g_flag_10hz) {
            g_flag_10hz = 0;

            /* 1. Echantillonner les capteurs */
            adc_sample(&g_adc);

            /* 2. Calculer les grandeurs energetiques */
            energy_update(&g_energy, &g_adc);

            /* 3. Mettre a jour la FSM (decide + applique) */
            fsm_update(&g_fsm, &g_energy, &g_loads, &g_alerts);
        }

        /* Affichage a 1 Hz */
        if (g_flag_1hz) {
            g_flag_1hz = 0;
            g_fsm.uptime_s++;
            display_status(&g_fsm, &g_energy, &g_loads);
        }
    }
}
```

---

## Sortie UART Attendue (Simulation)

```
╔═══════════════════════════════════════╗
║   TSAKITSAKY ENERGY v1.0              ║
║   Controleur Solaire Intelligent      ║
║   ARM Cortex-M0                       ║
║   (c) 2026 S. RAZAFINDRAKOTO          ║
╚═══════════════════════════════════════╝

Initialisation...
Systeme pret.

[3s]  NORMAL   | Bat:12.2V 60% | Sol:0.0W  | Load:0.0W  | L:000
[5s]  NORMAL   | Bat:12.4V 68% | Sol:12.5W | Load:18.3W | L:111
[10s] NORMAL   | Bat:12.6V 82% | Sol:38.2W | Load:18.3W | L:111
[15s] NORMAL   | Bat:12.7V 95% | Sol:45.0W | Load:18.3W | L:111
[20s] NORMAL   | Bat:12.5V 85% | Sol:28.0W | Load:18.3W | L:111
[30s] NORMAL   | Bat:12.2V 62% | Sol:5.0W  | Load:18.3W | L:111
[35s] LOW_BAT  | Bat:11.7V 28% | Sol:0.0W  | Load:12.0W | L:110
[40s] CRITICAL | Bat:11.3V 9%  | Sol:0.0W  | Load:6.0W  | L:100
[45s] SHUTDOWN | Bat:10.4V 0%  | Sol:0.0W  | Load:0.0W  | L:000
[60s] CRITICAL | Bat:11.5V 15% | Sol:8.0W  | Load:6.0W  | L:100
```

---

[<< Software Architecture](./04-Software-Architecture.md) | [Tests >>](./06-Tests-Validation.md)
