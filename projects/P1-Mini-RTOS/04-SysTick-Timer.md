# 4. Configuration du Timer SysTick

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index projet](./00-INDEX-PROJET.md) | [<< Context Switch](./03-Context-Switch.md) | [Implementation >>](./05-Implementation.md)

---

## 4.1 Role du SysTick dans le RTOS

Le SysTick genere une **interruption periodique** qui sert de "battement de coeur" au RTOS. A chaque tick, il declenche l'ordonnanceur.

```
 |←── tick ──→|←── tick ──→|←── tick ──→|
 |   Task 0   |   Task 1   |   Task 2   |
 ^             ^             ^
 SysTick      SysTick      SysTick
 IRQ          IRQ          IRQ
```

## 4.2 Registres SysTick

| Registre | Adresse | Fonction |
|----------|---------|----------|
| `SYST_CSR` | `0xE000E010` | Controle et statut |
| `SYST_RVR` | `0xE000E014` | Valeur de rechargement |
| `SYST_CVR` | `0xE000E018` | Valeur courante |

### Bits de SYST_CSR

| Bit | Nom | Description |
|-----|-----|-------------|
| 0 | ENABLE | 1 = Timer active |
| 1 | TICKINT | 1 = Interruption quand compteur atteint 0 |
| 2 | CLKSOURCE | 1 = Horloge processeur |

## 4.3 Configuration

```c
#define SYSTICK_FREQ_HZ    1000    // 1 ms par tick (1000 Hz)
#define CPU_FREQ_HZ        12000000 // 12 MHz (simulateur Keil)

void systick_init(void) {
    // Valeur de rechargement = (CPU_FREQ / TICK_FREQ) - 1
    SysTick->LOAD = (CPU_FREQ_HZ / SYSTICK_FREQ_HZ) - 1;  // 11999
    SysTick->VAL  = 0;        // Reset le compteur
    SysTick->CTRL = (1 << 2)  // Horloge processeur
                  | (1 << 1)  // Activer interruption
                  | (1 << 0); // Activer le timer

    // Definir la priorite de PendSV au minimum (3 sur M0)
    // Pour que le context switch ne preempte pas les autres ISR
    SCB->SHP[1] |= (0xC0 << 16);  // PendSV = priorite 3
}
```

## 4.4 Calcul du Time Slice

| Parametre | Valeur |
|-----------|--------|
| Horloge CPU (simulateur) | 12 MHz |
| Frequence SysTick | 1000 Hz (1 ms) |
| Valeur LOAD | 11 999 |
| Time slice par tache | 1 ms (ou configurable avec un compteur) |

Pour un time slice de **10 ms** (plus realiste) :

```c
volatile uint32_t tick_count = 0;
#define TIME_SLICE  10  // 10 ticks = 10 ms

void SysTick_Handler(void) {
    tick_count++;
    if (tick_count >= TIME_SLICE) {
        tick_count = 0;
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;  // Declencher PendSV
    }
}
```

---

[<< Context Switch](./03-Context-Switch.md) | [Implementation >>](./05-Implementation.md)
