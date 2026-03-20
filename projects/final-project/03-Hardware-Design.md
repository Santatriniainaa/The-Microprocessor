# 03 - Hardware Design : TSAKITSAKY ENERGY

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. Schema Bloc Electronique

```
 PANNEAU SOLAIRE 12V/50W
       │
       ├──[R1/R2 Divider]──→ ADC CH0 (V_panel : 0-3.3V)
       │
       ├──[INA219 #1]──→ I2C → MCU (I_panel)
       │
       ▼
 ┌─────────────┐
 │  MOSFET Q0  │←── PWM MCU (MPPT tracking)
 │  (IRF540N)  │
 └──────┬──────┘
        │
        ▼
 BATTERIE 12V/100Ah
       │
       ├──[R3/R4 Divider]──→ ADC CH1 (V_battery : 0-3.3V)
       │
       ├──[INA219 #2]──→ I2C → MCU (I_battery)
       │
       ├──[MOSFET Q1]←── GPIO MCU ──→ CHARGE 1 (Eclairage, 2A max)
       │
       ├──[MOSFET Q2]←── GPIO MCU ──→ CHARGE 2 (Chargeur, 2A max)
       │
       └──[MOSFET Q3]←── GPIO MCU ──→ CHARGE 3 (Radio/TV, 3A max)


 MCU (STM32F030 / Cortex-M0)
       │
       ├──→ UART TX/RX → LCD 16x2 (ou module GSM SIM800)
       │
       ├──→ GPIO → LED Verte (NORMAL)
       ├──→ GPIO → LED Orange (LOW_BATTERY)
       └──→ GPIO → LED Rouge (CRITICAL/ALERT)
```

## 2. Bill of Materials (BOM)

| # | Composant | Reference | Qte | Prix unit. | Total |
|---|-----------|-----------|-----|-----------|-------|
| 1 | MCU ARM Cortex-M0 | STM32F030F4P6 | 1 | $0.80 | $0.80 |
| 2 | Capteur courant/tension | INA219 | 2 | $1.00 | $2.00 |
| 3 | MOSFET N-channel 10A | IRF540N | 4 | $0.30 | $1.20 |
| 4 | Driver MOSFET | TC4427 | 2 | $0.50 | $1.00 |
| 5 | Regulateur 3.3V | AMS1117-3.3 | 1 | $0.15 | $0.15 |
| 6 | Cristal 8 MHz | HC-49S | 1 | $0.20 | $0.20 |
| 7 | Diode Schottky 5A | SS54 | 2 | $0.15 | $0.30 |
| 8 | Resistances (kit) | 0805 SMD | 20 | $0.02 | $0.40 |
| 9 | Condensateurs (kit) | 0805 SMD | 15 | $0.03 | $0.45 |
| 10 | Condensateur elec 100uF | 16V | 3 | $0.10 | $0.30 |
| 11 | LED 3mm (R/O/V) | - | 3 | $0.05 | $0.15 |
| 12 | Connecteurs a vis | 5.08mm | 8 | $0.15 | $1.20 |
| 13 | PCB 2 couches | 80x60mm | 1 | $1.50 | $1.50 |
| 14 | Boitier IP54 | ABS 120x80x40 | 1 | $2.00 | $2.00 |
| 15 | LCD 16x2 I2C | HD44780 | 1 | $2.50 | $2.50 |
| | | | | **Total BOM** | **$14.15** |

## 3. Diviseurs de Tension (ADC)

### Tension panneau (0-25V → 0-3.3V)

```
V_panel ──[R1=68kΩ]──┬──[R2=10kΩ]── GND
                      │
                      └──→ ADC CH0

V_adc = V_panel × R2/(R1+R2) = V_panel × 10/78 ≈ V_panel × 0.128
V_panel_max = 3.3V × 78/10 = 25.74V   ✓
Resolution = 25.74V / 4096 = 6.28 mV/bit
```

### Tension batterie (0-16V → 0-3.3V)

```
V_bat ──[R3=39kΩ]──┬──[R4=10kΩ]── GND
                    │
                    └──→ ADC CH1

V_adc = V_bat × 10/49 ≈ V_bat × 0.204
V_bat_max = 3.3V × 49/10 = 16.17V   ✓
Resolution = 16.17V / 4096 = 3.95 mV/bit
```

## 4. Protection

| Protection | Methode | Seuil |
|-----------|---------|-------|
| Surtension panneau | Diode TVS 30V | > 28V |
| Surtension batterie | Coupure MOSFET charge | > 14.4V |
| Decharge profonde | Coupure charges progressive | < 11.5V |
| Surcourant | Fusible + detection INA219 | > 8A total |
| Inversion polarite | MOSFET P-channel en entree | - |
| Surchauffe | NTC + coupure software | > 60°C |

---

[<< Business Plan](./02-Business-Plan.md) | [Software Architecture >>](./04-Software-Architecture.md)
