# 04 - Architecture Logicielle : TSAKITSAKY ENERGY

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. Structure des Fichiers

```
final-project/src/
├── config.h            Constantes, seuils, parametres systeme
├── adc.h / adc.c       Acquisition 4 canaux + moyennage
├── energy.h / energy.c  Calculs puissance, SOC batterie, MPPT
├── load.h / load.c     Controle 3 charges + priorite
├── alert.h / alert.c   Alertes, protection, LED status
├── display.h / display.c  Affichage UART/LCD + formatage
├── logger.h / logger.c   Historique energie (ring buffer)
├── fsm.h / fsm.c       Machine a etats principale
└── main.c              Init + boucle principale + SysTick ISR
```

## 2. Flux de Donnees (Pipeline)

```
 Toutes les 100 ms (SysTick 10 Hz) :

  ┌─────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐
  │ ADC     │────→│ ENERGY   │────→│ FSM      │────→│ LOAD     │
  │ Mesurer │     │ Calculer │     │ Decider  │     │ Appliquer│
  │ 4 canaux│     │ P,SOC    │     │ etat     │     │ ON/OFF   │
  └─────────┘     └────┬─────┘     └────┬─────┘     └──────────┘
                       │                │
                  ┌────▼─────┐     ┌────▼─────┐
                  │ LOGGER   │     │ ALERT    │
                  │ Historiq. │     │ Protect. │
                  └──────────┘     │ LED stat │
                                   └────┬─────┘
                                        │
                                   ┌────▼─────┐
                                   │ DISPLAY  │
                                   │ UART/LCD │
                                   └──────────┘

 Toutes les 1 seconde : mise a jour affichage
 Toutes les 60 secondes : log dans historique
```

## 3. Interruptions

| Source | Frequence | Handler | Action |
|--------|-----------|---------|--------|
| **SysTick** | 1 kHz | `SysTick_Handler` | Incremente tick_ms, flag 10Hz et 1Hz |
| **ADC EOC** | 10 Hz (declenche par timer) | - | Polling dans la boucle |
| **UART RX** | Asynchrone | `USART1_IRQHandler` | Commandes utilisateur |

## 4. Gestion Memoire

```
ROM (Flash) 16 kB                    RAM (SRAM) 4 kB
┌───────────────────┐                ┌────────────────────┐
│ .vectors (256 B)  │                │ .data (200 B)      │
│ .text (10 kB)     │                │ .bss (400 B)       │
│ .rodata (2 kB)    │                │ Ring buffer (2 kB) │
│ (libre ~4 kB)     │                │ Pile (1.2 kB)      │
└───────────────────┘                │ (libre 200 B)      │
                                     └────────────────────┘

Budget memoire :
  Code       : ~10 kB / 16 kB  (62%)
  Constantes :  ~2 kB / 16 kB  (12%)
  Variables  : ~600 B /  4 kB  (15%)
  Logger     :  ~2 kB /  4 kB  (50%)
  Pile       : ~1.2 kB / 4 kB  (30%)
```

## 5. Algorithme MPPT Simplifie (Perturb & Observe)

```
Toutes les 10 secondes :
  1. Mesurer P_actuelle = V_panel × I_panel
  2. Si P_actuelle > P_precedente :
       → continuer dans la meme direction (augmenter/diminuer duty)
     Sinon :
       → inverser la direction
  3. Ajuster duty cycle PWM (±1%)
  4. P_precedente = P_actuelle
```

## 6. Calcul SOC Batterie (State of Charge)

```
Methode : Tension en circuit ouvert (OCV) pour batterie plomb 12V

V_bat (V)  │  SOC (%)
───────────┼──────────
  12.70    │   100
  12.50    │    90
  12.42    │    80
  12.32    │    70
  12.20    │    60
  12.06    │    50
  11.90    │    40
  11.75    │    30
  11.58    │    20
  11.31    │    10
  10.50    │     0

Interpolation lineaire entre les points.
```

---

[<< Hardware](./03-Hardware-Design.md) | [Codebase >>](./05-Codebase.md)
