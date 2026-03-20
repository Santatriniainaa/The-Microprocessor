# 06 - Plan de Tests et Validation

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. Tests Unitaires (par module)

### 1.1 ADC (`adc.c`)

| Test | Entree | Sortie attendue | Statut |
|------|--------|----------------|--------|
| Lecture canal 0 | Simulation soleil midi | ADC ~3200 | [ ] |
| Lecture canal 0 | Simulation nuit | ADC ~0 | [ ] |
| Lecture canal 1 | Batterie 12.5V | ADC ~2612 | [ ] |
| Moyennage | 8 echantillons constants | Valeur identique | [ ] |
| Moyennage | 8 echantillons variables | Moyenne correcte | [ ] |

### 1.2 Energy (`energy.c`)

| Test | Entree | Sortie attendue | Statut |
|------|--------|----------------|--------|
| SOC batterie pleine | 12700 mV | 100% | [ ] |
| SOC batterie vide | 10500 mV | 0% | [ ] |
| SOC batterie 50% | 12060 mV | 50% | [ ] |
| Interpolation SOC | 12150 mV | ~55% | [ ] |
| Puissance solaire | 18V, 2.5A | 45000 mW | [ ] |
| Bilan net positif | Solar 45W, Load 18W | +27000 mW | [ ] |
| Bilan net negatif | Solar 0W, Load 18W | -18000 mW | [ ] |

### 1.3 FSM (`fsm.c`)

| Test | Etat actuel | Condition | Etat suivant | Statut |
|------|-------------|-----------|-------------|--------|
| Demarrage OK | STARTUP | timer > 20, SOC > 30% | NORMAL | [ ] |
| Demarrage bat faible | STARTUP | timer > 20, SOC < 30% | LOW_BAT | [ ] |
| Normal → Low | NORMAL | SOC < 30% | LOW_BAT | [ ] |
| Low → Normal | LOW_BAT | SOC > 40% | NORMAL | [ ] |
| Low → Critical | LOW_BAT | SOC < 10% | CRITICAL | [ ] |
| Critical → Shutdown | CRITICAL | Vbat < 10.5V | SHUTDOWN | [ ] |
| Shutdown → Reveil | SHUTDOWN | Solar > 1W, Vbat > 11.3V | CRITICAL | [ ] |
| Hysteresis | LOW_BAT | SOC = 35% | LOW_BAT (pas NORMAL) | [ ] |

### 1.4 Load (`load.c`)

| Test | Commande | GPIO attendu | Statut |
|------|----------|-------------|--------|
| Toutes OFF | set_all(0,0,0) | 0x00 | [ ] |
| Toutes ON | set_all(1,1,1) | 0xE0 (bits 5,6,7) | [ ] |
| Priorite haute seule | set_all(1,0,0) | 0x20 (bit 5) | [ ] |

---

## 2. Tests d'Integration (scenarios complets)

### Scenario A : Journee ensoleillee normale

```
Conditions : soleil 8h-18h, batterie initialement 60%
Attendu :
  - Matin : NORMAL, toutes charges ON, batterie charge
  - Midi : SOC monte a 90-100%
  - Soir : SOC descend progressivement
  - Nuit : LOW_BAT si SOC < 30%, couper charge 3
  - Aube : Reprise quand soleil revient
```

### Scenario B : Journee nuageuse

```
Conditions : soleil faible (20% nominal), batterie 40%
Attendu :
  - NORMAL brievement, puis LOW_BAT rapidement
  - Charges 1+2 seulement
  - Si prolonge : CRITICAL → charge 1 seule
```

### Scenario C : Panne batterie

```
Conditions : batterie a 10%, pas de soleil
Attendu :
  - CRITICAL → SHUTDOWN en quelques minutes
  - Toutes charges OFF
  - LED rouge clignotante
  - Reveil automatique au prochain lever de soleil
```

---

## 3. Verification dans Keil MDK

### Variables a observer (Watch Window)

| Variable | Module | Description |
|----------|--------|-------------|
| `g_fsm.state` | fsm | Etat courant |
| `g_energy.soc_percent` | energy | SOC batterie |
| `g_energy.v_battery_mv` | energy | Tension batterie |
| `g_energy.p_solar_mw` | energy | Puissance solaire |
| `LOAD_GPIO_REG` | load | Etat GPIO charges |
| `ALERT_LED_REG` | alert | Etat LEDs |
| `g_tick_ms` | main | Horloge systeme |

### Breakpoints recommandes

| Emplacement | Objectif |
|-------------|---------|
| `fsm_update()` debut | Verifier les transitions |
| `energy_update()` fin | Verifier les calculs |
| `SysTick_Handler` | Verifier la cadence |
| `display_status()` | Verifier la sortie UART |

---

## 4. Criteres d'Acceptation (Definition of Done)

- [ ] Les 5 etats FSM sont atteignables et les transitions correctes
- [ ] L'hysteresis empeche les oscillations entre etats
- [ ] Le SOC est calcule avec precision ±5%
- [ ] Les charges sont correctement controlees par priorite
- [ ] La sortie UART affiche les donnees chaque seconde
- [ ] Aucun HardFault apres 1 heure de simulation
- [ ] La consommation memoire reste dans les limites (16kB ROM, 4kB RAM)

---

[<< Codebase](./05-Codebase.md) | [Deploiement >>](./07-Deploiement.md)
