# P3 - Systeme de Controle Domotique ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Difficulte** : Intermediaire | **Chapitres** : 17-19
> **Simulation** : Keil MDK Cortex-M0 Simulator

---

## 1. Objectif Business

Simuler un **systeme domotique** complet (maison intelligente) gerant :
- **Capteurs** : temperature (ADC), luminosite (ADC), bouton (GPIO)
- **Actionneurs** : LED (GPIO), ventilateur (PWM), alarme (GPIO)
- **Logique de controle** : machine a etats finis (FSM)
- **Communication** : rapport d'etat via UART

**Marche cible** : IoT ($135B en 2025), domotique, batiment intelligent.

## 2. Architecture

```
  Capteurs (ADC simulee)           Logique                 Actionneurs (GPIO)
 ┌──────────────────┐       ┌────────────────┐       ┌──────────────────┐
 │ temperature_read()│──────→│                │──────→│ led_set(on/off)  │
 │ light_read()      │──────→│  FSM Controller│──────→│ fan_set(speed)   │
 │ button_read()     │──────→│  (5 etats)     │──────→│ alarm_set(on/off)│
 └──────────────────┘       └───────┬────────┘       └──────────────────┘
                                    │
                                    v
                            ┌───────────────┐
                            │ UART Reporter │
                            │ (etat + logs) │
                            └───────────────┘
```

## 3. Machine a Etats (FSM)

```
                    ┌──────────┐
         ┌─────────│   IDLE   │←────────────┐
         │         └────┬─────┘             │
         │              │ temp > 30         │ temp < 25
         │              v                   │
         │         ┌──────────┐             │
         │    ┌────│ COOLING  │─────────────┘
         │    │    └────┬─────┘
         │    │         │ light < 20
         │    │         v
         │    │    ┌──────────┐
         │    └────│  NIGHT   │────┐
         │         └────┬─────┘    │ light > 50
         │              │          v
         │              │    ┌──────────┐
         │  button───→  └────│   DAY    │
         │              │    └──────────┘
         │              v
         │         ┌──────────┐
         └─────────│  ALERT   │  (bouton = reset)
                   └──────────┘
```

## 4. Code Source Complet

### 4.1 `home_system.h`

```c
/*******************************************************************************
 * @file    home_system.h
 * @brief   Systeme domotique - Definitions et interfaces
 ******************************************************************************/
#ifndef HOME_SYSTEM_H
#define HOME_SYSTEM_H

#include <stdint.h>

/*--- Etats de la FSM ---*/
typedef enum {
    STATE_IDLE    = 0,
    STATE_COOLING = 1,
    STATE_NIGHT   = 2,
    STATE_DAY     = 3,
    STATE_ALERT   = 4
} home_state_t;

/*--- Seuils de configuration ---*/
#define TEMP_HIGH_THRESHOLD    30    /* degres C */
#define TEMP_LOW_THRESHOLD     25
#define LIGHT_LOW_THRESHOLD    20    /* % luminosite */
#define LIGHT_HIGH_THRESHOLD   50
#define TEMP_CRITICAL          45    /* declenchement alarme */

/*--- Actionneurs ---*/
#define FAN_OFF     0
#define FAN_LOW     1
#define FAN_HIGH    2

/*--- Structure d'etat du systeme ---*/
typedef struct {
    home_state_t state;
    int32_t      temperature;     /* degres C (simule) */
    int32_t      light_level;     /* 0-100 % (simule) */
    uint32_t     button_pressed;  /* 0 ou 1 */
    uint32_t     fan_speed;       /* FAN_OFF, FAN_LOW, FAN_HIGH */
    uint32_t     led_on;          /* 0 ou 1 */
    uint32_t     alarm_on;        /* 0 ou 1 */
    uint32_t     uptime_seconds;  /* temps ecoule */
} home_context_t;

/*--- API ---*/
void home_init(home_context_t *ctx);
void home_update(home_context_t *ctx);
void home_report(const home_context_t *ctx);

#endif
```

### 4.2 `sensors.h` / `sensors.c` - Capteurs simules

```c
/*--- sensors.h ---*/
#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

void     sensors_init(void);
int32_t  sensor_read_temperature(void);  /* retourne degres C */
int32_t  sensor_read_light(void);        /* retourne 0-100 % */
uint32_t sensor_read_button(void);       /* retourne 0 ou 1 */
void     sensor_simulate_tick(void);     /* avance la simulation */

#endif
```

```c
/*--- sensors.c ---*/
#include "sensors.h"

/* Donnees de simulation : sequences realistes */
static const int32_t temp_sequence[] = {
    22, 23, 25, 27, 29, 31, 33, 35, 38, 40, 42, 45, 44, 40,
    37, 35, 33, 30, 28, 26, 24, 22, 21, 20, 19, 20, 21, 22
};
#define TEMP_SEQ_LEN  (sizeof(temp_sequence) / sizeof(temp_sequence[0]))

static const int32_t light_sequence[] = {
    5, 5, 8, 15, 30, 55, 70, 85, 90, 95, 90, 85, 75, 60,
    45, 30, 18, 10, 5, 3, 2, 2, 3, 5
};
#define LIGHT_SEQ_LEN (sizeof(light_sequence) / sizeof(light_sequence[0]))

static volatile uint32_t sim_tick = 0;
static volatile uint32_t button_state = 0;

/* Simule un bouton appuye aux ticks 50, 100, 150 */
static const uint32_t button_ticks[] = {50, 100, 150};
#define BTN_TICKS_LEN 3

void sensors_init(void) {
    sim_tick = 0;
    button_state = 0;
}

int32_t sensor_read_temperature(void) {
    return temp_sequence[sim_tick % TEMP_SEQ_LEN];
}

int32_t sensor_read_light(void) {
    return light_sequence[sim_tick % LIGHT_SEQ_LEN];
}

uint32_t sensor_read_button(void) {
    uint32_t pressed = 0;
    for (uint32_t i = 0; i < BTN_TICKS_LEN; i++) {
        if (sim_tick == button_ticks[i]) pressed = 1;
    }
    return pressed;
}

void sensor_simulate_tick(void) {
    sim_tick++;
}
```

### 4.3 `actuators.h` / `actuators.c` - Actionneurs simules

```c
/*--- actuators.h ---*/
#ifndef ACTUATORS_H
#define ACTUATORS_H
#include <stdint.h>

void actuator_init(void);
void actuator_led(uint32_t on);
void actuator_fan(uint32_t speed);   /* 0=off, 1=low, 2=high */
void actuator_alarm(uint32_t on);

/* Registres simules (observables dans Memory Window) */
extern volatile uint32_t GPIO_LED_REG;
extern volatile uint32_t GPIO_FAN_REG;
extern volatile uint32_t GPIO_ALARM_REG;

#endif
```

```c
/*--- actuators.c ---*/
#include "actuators.h"

volatile uint32_t GPIO_LED_REG   = 0;
volatile uint32_t GPIO_FAN_REG   = 0;
volatile uint32_t GPIO_ALARM_REG = 0;

void actuator_init(void) {
    GPIO_LED_REG   = 0;
    GPIO_FAN_REG   = 0;
    GPIO_ALARM_REG = 0;
}

void actuator_led(uint32_t on) {
    GPIO_LED_REG = on ? 0xFF : 0x00;
}

void actuator_fan(uint32_t speed) {
    switch (speed) {
        case 0:  GPIO_FAN_REG = 0x00; break;  /* OFF */
        case 1:  GPIO_FAN_REG = 0x0F; break;  /* LOW (50% PWM simule) */
        case 2:  GPIO_FAN_REG = 0xFF; break;  /* HIGH (100%) */
        default: GPIO_FAN_REG = 0x00; break;
    }
}

void actuator_alarm(uint32_t on) {
    GPIO_ALARM_REG = on ? 0xFF : 0x00;
}
```

### 4.4 `home_system.c` - Logique FSM

```c
/*******************************************************************************
 * @file    home_system.c
 * @brief   Machine a etats finis du systeme domotique
 ******************************************************************************/
#include "home_system.h"
#include "sensors.h"
#include "actuators.h"

void home_init(home_context_t *ctx) {
    ctx->state          = STATE_IDLE;
    ctx->temperature    = 0;
    ctx->light_level    = 0;
    ctx->button_pressed = 0;
    ctx->fan_speed      = FAN_OFF;
    ctx->led_on         = 0;
    ctx->alarm_on       = 0;
    ctx->uptime_seconds = 0;

    sensors_init();
    actuator_init();
}

void home_update(home_context_t *ctx) {
    /* 1. Lire les capteurs */
    ctx->temperature    = sensor_read_temperature();
    ctx->light_level    = sensor_read_light();
    ctx->button_pressed = sensor_read_button();

    /* 2. Transition de la FSM */
    switch (ctx->state) {

    case STATE_IDLE:
        ctx->fan_speed = FAN_OFF;
        ctx->led_on    = 0;
        ctx->alarm_on  = 0;
        if (ctx->temperature >= TEMP_CRITICAL) {
            ctx->state = STATE_ALERT;
        } else if (ctx->temperature >= TEMP_HIGH_THRESHOLD) {
            ctx->state = STATE_COOLING;
        } else if (ctx->light_level < LIGHT_LOW_THRESHOLD) {
            ctx->state = STATE_NIGHT;
        }
        break;

    case STATE_COOLING:
        ctx->fan_speed = (ctx->temperature >= 38) ? FAN_HIGH : FAN_LOW;
        ctx->led_on    = 0;
        ctx->alarm_on  = 0;
        if (ctx->temperature >= TEMP_CRITICAL) {
            ctx->state = STATE_ALERT;
        } else if (ctx->temperature < TEMP_LOW_THRESHOLD) {
            ctx->state = STATE_IDLE;
        } else if (ctx->light_level < LIGHT_LOW_THRESHOLD) {
            ctx->state = STATE_NIGHT;
        }
        break;

    case STATE_NIGHT:
        ctx->led_on    = 1;
        ctx->fan_speed = FAN_OFF;
        ctx->alarm_on  = 0;
        if (ctx->temperature >= TEMP_CRITICAL) {
            ctx->state = STATE_ALERT;
        } else if (ctx->light_level >= LIGHT_HIGH_THRESHOLD) {
            ctx->state = STATE_DAY;
        }
        break;

    case STATE_DAY:
        ctx->led_on    = 0;
        ctx->fan_speed = FAN_OFF;
        ctx->alarm_on  = 0;
        if (ctx->temperature >= TEMP_CRITICAL) {
            ctx->state = STATE_ALERT;
        } else if (ctx->temperature >= TEMP_HIGH_THRESHOLD) {
            ctx->state = STATE_COOLING;
        } else if (ctx->light_level < LIGHT_LOW_THRESHOLD) {
            ctx->state = STATE_NIGHT;
        }
        break;

    case STATE_ALERT:
        ctx->alarm_on  = 1;
        ctx->fan_speed = FAN_HIGH;
        ctx->led_on    = (ctx->uptime_seconds & 1); /* clignotement */
        if (ctx->button_pressed) {
            ctx->alarm_on = 0;
            ctx->state = STATE_IDLE;
        }
        break;
    }

    /* 3. Appliquer aux actionneurs */
    actuator_led(ctx->led_on);
    actuator_fan(ctx->fan_speed);
    actuator_alarm(ctx->alarm_on);

    /* 4. Avancer la simulation */
    ctx->uptime_seconds++;
    sensor_simulate_tick();
}
```

### 4.5 `main.c`

```c
/*******************************************************************************
 * @file    main.c
 * @brief   Programme principal du systeme domotique
 ******************************************************************************/
#include "home_system.h"
#include "uart_io.h"

static const char *state_names[] = {
    "IDLE", "COOLING", "NIGHT", "DAY", "ALERT"
};

void home_report(const home_context_t *ctx) {
    uart_send_string("[T=");
    uart_send_int(ctx->uptime_seconds);
    uart_send_string("s] Etat:");
    uart_send_string(state_names[ctx->state]);
    uart_send_string(" Temp:");
    uart_send_int(ctx->temperature);
    uart_send_string("C Lum:");
    uart_send_int(ctx->light_level);
    uart_send_string("% Fan:");
    uart_send_int(ctx->fan_speed);
    uart_send_string(" LED:");
    uart_send_int(ctx->led_on);
    uart_send_string(" Alarm:");
    uart_send_int(ctx->alarm_on);
    uart_send_string("\r\n");
}

/* Timer SysTick : 1 seconde par tick de simulation */
static volatile uint32_t tick_flag = 0;

void SysTick_Handler(void) {
    tick_flag = 1;
}

int main(void) {
    home_context_t home;

    uart_init(115200);
    home_init(&home);

    uart_send_string("=== Systeme Domotique Cortex-M0 ===\r\n");
    uart_send_string("Demarrage...\r\n\r\n");

    /* Configurer SysTick pour 1 seconde (12 MHz) */
    *(volatile uint32_t *)0xE000E014 = 12000000 - 1;
    *(volatile uint32_t *)0xE000E018 = 0;
    *(volatile uint32_t *)0xE000E010 = 0x07;

    while (1) {
        if (tick_flag) {
            tick_flag = 0;
            home_update(&home);
            home_report(&home);
        }
    }
}
```

### 4.6 Scenario de test attendu (sortie UART)

```
=== Systeme Domotique Cortex-M0 ===
Demarrage...

[T=0s]  Etat:IDLE    Temp:22C Lum:5%  Fan:0 LED:0 Alarm:0
[T=1s]  Etat:NIGHT   Temp:23C Lum:5%  Fan:0 LED:1 Alarm:0
[T=4s]  Etat:NIGHT   Temp:29C Lum:30% Fan:0 LED:1 Alarm:0
[T=5s]  Etat:DAY     Temp:31C Lum:55% Fan:0 LED:0 Alarm:0
[T=6s]  Etat:COOLING Temp:33C Lum:70% Fan:1 LED:0 Alarm:0
[T=8s]  Etat:COOLING Temp:38C Lum:90% Fan:2 LED:0 Alarm:0
[T=11s] Etat:ALERT   Temp:45C Lum:85% Fan:2 LED:1 Alarm:1
[T=50s] Etat:IDLE    Temp:22C Lum:5%  Fan:0 LED:0 Alarm:0  (button reset)
```

---

[Retour a l'index](../00-INDEX-PROJET.md)
