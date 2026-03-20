# 2. Ordonnanceur Round-Robin (Scheduler)

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index projet](./00-INDEX-PROJET.md) | [<< Architecture](./01-Architecture-Systeme.md) | [Context Switch >>](./03-Context-Switch.md)

---

## 2.1 Principe Round-Robin

Chaque tache reçoit le processeur pendant une **tranche de temps** (time slice) egale. Quand la tranche expire, le scheduler passe a la tache suivante de maniere cyclique.

```
Task0 → Task1 → Task2 → Task0 → Task1 → Task2 → ...
```

## 2.2 Structure de Donnees

```c
#define NUM_TASKS    3
#define STACK_SIZE   256    // mots de 32 bits par tache

typedef struct {
    uint32_t *sp;                  // Stack pointer sauvegarde
    uint32_t stack[STACK_SIZE];    // Pile privee de la tache
} TCB_t;

TCB_t tcb[NUM_TASKS];             // Tableau des TCB
volatile uint32_t current_task;    // Index de la tache courante
```

## 2.3 Initialisation des Piles

Avant de demarrer, chaque pile de tache doit etre **pre-remplie** avec un cadre d'exception simule :

```c
void scheduler_init(void) {
    for (int i = 0; i < NUM_TASKS; i++) {
        // Le SP pointe vers le "sommet" de la pile pre-remplie
        // Simuler le cadre que le CPU sauvegarde automatiquement
        uint32_t *sp = &tcb[i].stack[STACK_SIZE];

        *(--sp) = 0x01000000;          // xPSR (bit Thumb = 1)
        *(--sp) = (uint32_t)task_table[i]; // PC (adresse de la tache)
        *(--sp) = 0xFFFFFFFD;          // LR (EXC_RETURN Thread+PSP)
        *(--sp) = 0;                    // R12
        *(--sp) = 0;                    // R3
        *(--sp) = 0;                    // R2
        *(--sp) = 0;                    // R1
        *(--sp) = 0;                    // R0

        // Registres sauvegardes manuellement (R4-R11)
        for (int j = 0; j < 8; j++) {
            *(--sp) = 0;                // R4-R11
        }

        tcb[i].sp = sp;
    }
    current_task = 0;
}
```

### Schema de la pile pre-remplie

```
Adresses hautes (fond de pile)
+------------------+
|  xPSR  (Thumb=1) |  ← Sauvegarde auto par CPU
|  PC    (task_fn)  |
|  LR    (EXC_RET)  |
|  R12              |
|  R3               |
|  R2               |
|  R1               |
|  R0               |
+------------------+
|  R11              |  ← Sauvegarde manuelle
|  R10              |
|  R9               |
|  R8               |
|  R7               |
|  R6               |
|  R5               |
|  R4               |
+------------------+ ← tcb[i].sp (pointe ici)
Adresses basses (sommet de pile)
```

## 2.4 Selection de la Tache Suivante

```c
void scheduler_next(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}
```

## 2.5 Declenchement du Changement

```c
// Handler SysTick : declenche PendSV
void SysTick_Handler(void) {
    // Mettre PendSV en attente
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
```

> Le SysTick ne fait **pas** le changement de contexte lui-meme. Il met **PendSV** en attente. PendSV s'executera quand aucune autre ISR n'est active.

---

[<< Architecture](./01-Architecture-Systeme.md) | [Context Switch >>](./03-Context-Switch.md)
