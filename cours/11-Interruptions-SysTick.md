# Chapitre 11 : Interruptions et Timer SysTick

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 10](./10-Modele-Exception.md) | [Chapitre 12 >>](./12-Programmation-C.md)

---

## Objectifs

> Configurer et utiliser les interruptions via le NVIC, et programmer le timer SysTick.

---

## 11.1 Polling vs Interruptions

| Methode | Principe | Avantages | Inconvenients |
|---------|----------|-----------|---------------|
| **Polling** | Le CPU verifie en boucle | Simple | Gaspille du CPU |
| **Interruptions** | Le peripherique signale | Efficace, reactif | Plus complexe |

---

## 11.2 NVIC (Nested Vectored Interrupt Controller)

Le NVIC est **integre** au processeur Cortex-M0 :
- Jusqu'a **32 interruptions externes** (IRQ0-IRQ31)
- **4 niveaux** de priorite (2 bits)
- Supporte la **preemption** et le **tail-chaining**

### Registres NVIC (dans l'espace systeme 0xE000E000+)

| Registre | Adresse | Fonction |
|----------|---------|----------|
| **ISER** | 0xE000E100 | Interrupt Set Enable (activer) |
| **ICER** | 0xE000E180 | Interrupt Clear Enable (desactiver) |
| **ISPR** | 0xE000E200 | Interrupt Set Pending (forcer pending) |
| **ICPR** | 0xE000E280 | Interrupt Clear Pending (effacer pending) |
| **IPR0-7** | 0xE000E400-0xE000E41C | Interrupt Priority (priorite, 8 registres) |

> Chaque bit de ISER/ICER correspond a une IRQ. Bit 0 = IRQ0, bit 1 = IRQ1, etc.

### Priorite

Dans les registres IPR, chaque interruption a **8 bits** mais seuls les **2 bits de poids fort** (bits 7-6) sont utilises sur Cortex-M0 :

```
Bits : [7:6] = priorite (0-3)
       [5:0] = ignores (lus comme 0)

Priorite 0 = 0x00 (plus haute)
Priorite 1 = 0x40
Priorite 2 = 0x80
Priorite 3 = 0xC0 (plus basse)
```

---

## 11.3 Configuration des Interruptions

### Etapes

```
1. Configurer le peripherique (GPIO, Timer, UART...)
2. Activer l'interruption du peripherique (registre du peripherique)
3. Definir la priorite dans le NVIC (IPR)
4. Activer l'IRQ dans le NVIC (ISER)
5. Activer globalement les interruptions (PRIMASK = 0)
```

### Fonctions CMSIS

```c
NVIC_EnableIRQ(IRQn);            // Activer IRQ dans ISER
NVIC_DisableIRQ(IRQn);           // Desactiver dans ICER
NVIC_SetPriority(IRQn, prio);   // Definir priorite
NVIC_GetPriority(IRQn);          // Lire priorite
NVIC_SetPendingIRQ(IRQn);       // Forcer pending
NVIC_ClearPendingIRQ(IRQn);     // Effacer pending

__enable_irq();                  // CPSIE I (PRIMASK = 0)
__disable_irq();                 // CPSID I (PRIMASK = 1)
```

---

## 11.4 Ecriture d'un Handler

```c
// Nom du handler = nom dans la table des vecteurs
void TIM0_IRQHandler(void) {
    // 1. Traiter l'interruption
    // ... code de traitement ...

    // 2. Acquitter l'interruption (clear pending flag du peripherique)
    TIMER0->ICR = 1;    // Clear interrupt flag
}
```

> **Important** : Toujours **acquitter** (clear) le flag d'interruption du peripherique, sinon le handler sera appele en boucle infinie !

---

## 11.5 Timer SysTick

Timer **24 bits** a decompte, integre au processeur Cortex-M0.

### Registres SysTick

| Registre | Adresse | Description |
|----------|---------|-------------|
| **SYST_CSR** | 0xE000E010 | Control and Status Register |
| **SYST_RVR** | 0xE000E014 | Reload Value Register (valeur de rechargement) |
| **SYST_CVR** | 0xE000E018 | Current Value Register (valeur courante) |
| **SYST_CALIB** | 0xE000E01C | Calibration Register |

### SYST_CSR (Control and Status)

| Bit | Nom | Description |
|-----|-----|-------------|
| 16 | COUNTFLAG | 1 si le compteur a atteint 0 depuis la derniere lecture |
| 2 | CLKSOURCE | 0 = horloge externe, 1 = horloge processeur |
| 1 | TICKINT | 1 = generer une interruption quand le compteur atteint 0 |
| 0 | ENABLE | 1 = activer le timer |

### Fonctionnement

```
1. Charger la valeur de rechargement dans RVR
2. Le compteur (CVR) decremente a chaque cycle d'horloge
3. Quand CVR atteint 0 :
   - COUNTFLAG = 1
   - Si TICKINT = 1, interruption SysTick generee
   - CVR recharge avec la valeur de RVR
   - Le cycle recommence
```

### Configuration en C

```c
// Configurer SysTick pour une interruption toutes les 1 ms
// Horloge CPU = 48 MHz → 48000 cycles = 1 ms
SysTick->LOAD = 48000 - 1;        // Valeur de rechargement
SysTick->VAL  = 0;                 // Reset compteur
SysTick->CTRL = (1 << 2) |        // Horloge processeur
                (1 << 1) |        // Activer interruption
                (1 << 0);         // Activer timer

// Handler
void SysTick_Handler(void) {
    // Appele toutes les 1 ms
    ms_counter++;
}
```

### Applications du SysTick

| Application | Description |
|-------------|-------------|
| Delais precis | Attendre N millisecondes |
| Base de temps RTOS | Tick systeme pour l'ordonnanceur |
| Mesure de temps | Mesurer la duree d'execution |
| Clignotement LED | Basculer une LED a intervalle regulier |

---

## Resume

- **NVIC** : controleur integre, 32 IRQs, 4 niveaux de priorite
- **Registres** : ISER (activer), ICER (desactiver), IPR (priorite)
- **Etapes** : configurer peripherique → priorite → activer IRQ → activer global
- **Handler** : toujours acquitter le flag d'interruption
- **SysTick** : timer 24 bits, registres CSR/RVR/CVR
- **Formule** : `RVR = (frequence × periode) - 1`

---

[<< Chapitre 10](./10-Modele-Exception.md) | [Chapitre 12 : Programmation en C >>](./12-Programmation-C.md)
