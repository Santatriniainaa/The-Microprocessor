# 01 - Architecture Systeme TSAKITSAKY ENERGY

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. Diagramme Systeme Complet

```
                    ┌─────────────────────┐
    ☀ Panneau ──ADC→│                     │──GPIO──→ [CHARGE 1] Eclairage LED
    Solaire    CH0  │                     │          Priorite HAUTE
                    │   ARM Cortex-M0     │
    🔋 Batterie─ADC→│   TSAKITSAKY v1.0   │──GPIO──→ [CHARGE 2] Chargeur tel
    12V/100Ah  CH1  │                     │          Priorite MOYENNE
                    │  ┌──────┐ ┌──────┐  │
    Courant ───ADC→ │  │MPPT  │ │Energy│  │──GPIO──→ [CHARGE 3] Radio/TV
    Panneau    CH2  │  │Track │ │Stats │  │          Priorite BASSE
                    │  └──────┘ └──────┘  │
    Courant ───ADC→ │                     │──UART──→ LCD 16x2 / Module GSM
    Charges    CH3  │  SysTick: 1kHz      │
                    │  Boucle ctrl: 10Hz  │──GPIO──→ LED Status (3 couleurs)
                    └─────────────────────┘
```

## 2. Modules Logiciels

```
┌─────────────────────────────────────────────────────┐
│                    main.c                            │
│              Boucle principale                       │
│     init → mesurer → decider → agir → reporter      │
└──────────┬──────────┬──────────┬──────────┬─────────┘
           │          │          │          │
┌──────────▼──┐ ┌─────▼────┐ ┌──▼───────┐ ┌▼─────────┐
│  adc.h/c    │ │ energy.h/c│ │ load.h/c │ │ uart_io.h│
│ Acquisition │ │ Calculs   │ │ Controle │ │ Affichage│
│ 4 canaux    │ │ puissance │ │ 3 charges│ │ rapports │
│ moyennage   │ │ SOC batt  │ │ priorite │ │ alertes  │
└─────────────┘ │ MPPT      │ │ PWM sim  │ └──────────┘
                │ historique│ └──────────┘
                └───────────┘
┌─────────────┐ ┌───────────┐
│  alert.h/c  │ │ config.h  │
│ Seuils      │ │ Parametres│
│ Protection  │ │ Seuils    │
│ LED status  │ │ Constantes│
└─────────────┘ └───────────┘
```

## 3. Machine a Etats Principale (Energy Manager)

```
                            Demarrage
                               │
                               ▼
                        ┌──────────────┐
                   ┌───→│   STARTUP    │
                   │    │ Init capteurs│
                   │    │ Test batterie│
                   │    └──────┬───────┘
                   │           │ bat_ok
                   │           ▼
                   │    ┌──────────────┐    bat < 20%    ┌──────────────┐
                   │    │   NORMAL     │────────────────→│  LOW_BATTERY │
                   │    │ Toutes loads │                 │ Load3 OFF    │
                   │    │ MPPT actif   │◀───────────────│ Load2 reduit │
                   │    └──────┬───────┘    bat > 40%   └──────┬───────┘
                   │           │                               │
                   │           │ bat < 10%                     │ bat < 10%
                   │           ▼                               ▼
                   │    ┌──────────────┐              ┌──────────────┐
                   │    │  CRITICAL    │              │  CRITICAL    │
                   │    │ Load2+3 OFF  │◀─────────────│              │
                   │    │ Load1 reduit │              └──────────────┘
                   │    └──────┬───────┘
                   │           │ bat < 5%
                   │           ▼
                   │    ┌──────────────┐
                   │    │  SHUTDOWN    │
                   │    │ Tout OFF     │
                   └────│ Reveil si    │
                        │ soleil+charge│
                        └──────────────┘
```

## 4. Chronogramme d'une Journee Type

```
Heure   Soleil  Bat%   Etat       Load1  Load2  Load3
─────   ──────  ────   ─────────  ─────  ─────  ─────
05:00   0W      35%    LOW_BAT    ON     OFF    OFF
06:00   10W     33%    LOW_BAT    ON     OFF    OFF
08:00   40W     40%    NORMAL     ON     ON     ON
10:00   50W     65%    NORMAL     ON     ON     ON
12:00   50W     85%    NORMAL     ON     ON     ON
14:00   45W     95%    NORMAL     ON     ON     ON
16:00   30W     90%    NORMAL     ON     ON     ON
18:00   5W      80%    NORMAL     ON     ON     ON
20:00   0W      70%    NORMAL     ON     ON     ON
22:00   0W      55%    NORMAL     ON     ON     OFF
00:00   0W      40%    NORMAL     ON     ON     OFF
02:00   0W      25%    LOW_BAT    ON     OFF    OFF
04:00   0W      18%    CRITICAL   DIM    OFF    OFF
```

## 5. Specifications Techniques

| Parametre | Valeur |
|-----------|--------|
| MCU | ARM Cortex-M0 @ 48 MHz |
| ADC | 4 canaux, 12 bits, 10 Hz sampling |
| Tension panneau | 0-25V (diviseur resistif → 0-3.3V) |
| Tension batterie | 0-16V (diviseur resistif → 0-3.3V) |
| Courant max par charge | 5A (MOSFET N-channel) |
| Communication | UART 115200 baud |
| Consommation controleur | < 50 mW |
| Temperature operation | -10°C a +60°C |
| Protection | Surtension, surcourant, temperature |

---

[Retour au pitch](./00-PITCH.md) | [Business Plan >>](./02-Business-Plan.md)
