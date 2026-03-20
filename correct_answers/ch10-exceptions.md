# Corriges - Chapitre 10 : Exceptions et Interruptions

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Simulation : Timer SysTick

> **Enonce** : Configurer SysTick pour generer une interruption periodique et incrementer un compteur.

### Programme en C

```c
#include <stdint.h>

// Registres SysTick
#define SYST_CSR  (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR  (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR  (*(volatile uint32_t *)0xE000E018)

volatile uint32_t tick_count = 0;
volatile uint32_t seconds = 0;

void SysTick_Handler(void) {
    tick_count++;
    if (tick_count >= 1000) {   // 1000 ticks = 1 seconde
        tick_count = 0;
        seconds++;
    }
}

int main(void) {
    // Configurer SysTick pour 1 ms (a 12 MHz)
    SYST_RVR = 12000 - 1;      // Reload = 11999
    SYST_CVR = 0;               // Reset compteur
    SYST_CSR = 0x07;            // Enable + IRQ + CPU Clock

    while (1) {
        // Le programme principal tourne
        // tick_count et seconds sont mis a jour par l'ISR
        __asm("nop");
    }
}
```

### Programme en assembleur

```asm
    .syntax unified

    .bss
    .global tick_count
    .balign 4
tick_count: .space 4

    .global seconds
    .balign 4
seconds: .space 4

    .text
    .balign 2

// SysTick Handler
    .global SysTick_Handler
    .thumb_func
SysTick_Handler:
    push {r4, lr}

    ldr  r0, =tick_count
    ldr  r1, [r0]
    adds r1, #1
    str  r1, [r0]             // tick_count++

    ldr  r2, =1000
    cmp  r1, r2
    blt  systick_done

    movs r1, #0
    str  r1, [r0]             // tick_count = 0

    ldr  r0, =seconds
    ldr  r1, [r0]
    adds r1, #1
    str  r1, [r0]             // seconds++

systick_done:
    pop  {r4, pc}

// Programme principal
    .global asm_main
    .thumb_func
asm_main:
    // Configurer SysTick
    ldr  r0, =0xE000E014      // SYST_RVR
    ldr  r1, =11999           // 12000 - 1
    str  r1, [r0]

    ldr  r0, =0xE000E018      // SYST_CVR
    movs r1, #0
    str  r1, [r0]

    ldr  r0, =0xE000E010      // SYST_CSR
    movs r1, #7               // Enable + IRQ + CPU clock
    str  r1, [r0]

L0: b L0                      // boucle principale
    .size asm_main,.-asm_main
    .end
```

### Verification dans Keil

| Variable | Comportement attendu |
|----------|---------------------|
| `tick_count` | Incremente de 0 a 999 puis revient a 0 |
| `seconds` | Incremente toutes les 1000 ms |
| `SYST_CVR` (0xE000E018) | Decremente de 11999 a 0 puis recharge |

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
