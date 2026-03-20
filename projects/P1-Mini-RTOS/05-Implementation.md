# 5. Implementation Complete du Mini-RTOS

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index projet](./00-INDEX-PROJET.md) | [<< SysTick](./04-SysTick-Timer.md) | [Rapport >>](./06-Rapport-Projet.md)

---

## 5.1 scheduler.h

```c
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define NUM_TASKS    3
#define STACK_SIZE   256   // mots de 32 bits (1 kB par tache)

typedef struct {
    uint32_t *sp;                  // Pointeur de pile sauvegarde
    uint32_t stack[STACK_SIZE];    // Pile privee
} TCB_t;

extern TCB_t tcb[NUM_TASKS];
extern volatile uint32_t current_task;

void scheduler_init(void);
void scheduler_launch(void);

#endif
```

## 5.2 scheduler.c

```c
#include "scheduler.h"
#include "tasks.h"

TCB_t tcb[NUM_TASKS];
volatile uint32_t current_task = 0;

// Table des fonctions de taches
static void (*task_table[NUM_TASKS])(void) = {
    task0, task1, task2
};

void scheduler_init(void) {
    for (int i = 0; i < NUM_TASKS; i++) {
        uint32_t *sp = &tcb[i].stack[STACK_SIZE];

        // Cadre d'exception simule (sauvegarde auto par CPU)
        *(--sp) = 0x01000000;              // xPSR (Thumb bit)
        *(--sp) = (uint32_t)task_table[i]; // PC
        *(--sp) = 0x00000000;              // LR
        *(--sp) = 0x00000000;              // R12
        *(--sp) = 0x00000000;              // R3
        *(--sp) = 0x00000000;              // R2
        *(--sp) = 0x00000000;              // R1
        *(--sp) = 0x00000000;              // R0

        // Registres sauvegardes manuellement (R11 a R4)
        for (int j = 0; j < 8; j++) {
            *(--sp) = 0x00000000;          // R11,R10,R9,R8,R7,R6,R5,R4
        }

        tcb[i].sp = sp;
    }
    current_task = 0;
}
```

## 5.3 tasks.h / tasks.c

```c
// tasks.h
#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

extern volatile uint32_t task0_counter;
extern volatile uint32_t task1_result;
extern volatile uint32_t task2_output;

void task0(void);
void task1(void);
void task2(void);

#endif
```

```c
// tasks.c
#include "tasks.h"

volatile uint32_t task0_counter = 0;
volatile uint32_t task1_result  = 0;
volatile uint32_t task2_output  = 0;

// Task 0 : Incremente un compteur
void task0(void) {
    while (1) {
        task0_counter++;
    }
}

// Task 1 : Calcul simple (somme cumulative)
void task1(void) {
    uint32_t i = 0;
    while (1) {
        task1_result += i;
        i++;
        if (i > 100) i = 0;
    }
}

// Task 2 : Simule une sortie (toggle d'un registre)
void task2(void) {
    while (1) {
        task2_output ^= 0xFF;  // toggle les 8 bits bas
    }
}
```

## 5.4 main.c

```c
#include "scheduler.h"
#include "tasks.h"

// Registres systeme
#define SCB_ICSR     (*(volatile uint32_t *)0xE000ED04)
#define PENDSVSET    (1 << 28)

#define SYSTICK_CSR  (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_RVR  (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_CVR  (*(volatile uint32_t *)0xE000E018)

#define SCB_SHPR3   (*(volatile uint32_t *)0xE000ED20)

void systick_init(void) {
    SYSTICK_RVR = 12000 - 1;        // 1 ms a 12 MHz
    SYSTICK_CVR = 0;
    SYSTICK_CSR = 0x07;              // Enable + IRQ + CPU clock

    // PendSV a la priorite la plus basse (bits 23:22 = 11)
    SCB_SHPR3 |= (0xC0 << 16);
}

volatile uint32_t systick_count = 0;

void SysTick_Handler(void) {
    systick_count++;
    // Declencher PendSV pour le changement de contexte
    SCB_ICSR |= PENDSVSET;
}

int main(void) {
    scheduler_init();
    systick_init();

    // Charger le contexte de la premiere tache et demarrer
    scheduler_launch();

    // Ne devrait jamais arriver ici
    while (1);
}
```

## 5.5 Verification dans Keil

### Points a observer dans le debugger

| Element | Comment verifier |
|---------|-----------------|
| **task0_counter** | Watch Window : doit incrementer |
| **task1_result** | Watch Window : doit augmenter |
| **task2_output** | Watch Window : doit alterner 0x00/0xFF |
| **current_task** | Watch Window : doit cycler 0→1→2→0 |
| **SP (R13)** | Register Window : change a chaque switch |
| **SysTick** | Memory a 0xE000E018 : decremente |

### Breakpoints utiles

1. `SysTick_Handler` : verifier que le tick fonctionne
2. `PendSV_Handler` : verifier le context switch
3. Debut de chaque tache : verifier que les 3 s'executent

---

[<< SysTick](./04-SysTick-Timer.md) | [Rapport >>](./06-Rapport-Projet.md)
