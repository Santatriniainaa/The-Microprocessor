# 1. Architecture du Mini-RTOS

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index projet](./00-INDEX-PROJET.md)

---

## 1.1 Vue d'ensemble

```
+------------------------------------------------------------------+
|                        Mini-RTOS                                  |
|                                                                    |
|  +-------------------+    +------------------+                     |
|  |    Scheduler       |    |   SysTick ISR    |                     |
|  | (Round-Robin)      |<---|  (declencheur)   |                     |
|  +--------+----------+    +------------------+                     |
|           |                                                        |
|   +-------v-------+                                                |
|   | Context Switch |  ← Sauvegarde/Restauration des registres     |
|   +---+---+---+---+                                                |
|       |   |   |                                                    |
|   +---v-+ +-v---+ +--v--+                                          |
|   |Task1| |Task2| |Task3|   ← Taches independantes                |
|   |Pile1| |Pile2| |Pile3|   ← Chaque tache a sa propre pile       |
|   +-----+ +-----+ +-----+                                          |
+------------------------------------------------------------------+
```

## 1.2 Composants du Systeme

### TCB (Task Control Block)

Chaque tache est representee par un **TCB** :

```c
typedef struct {
    uint32_t *stack_pointer;   // Pointeur de pile de la tache
    void (*task_func)(void);   // Pointeur vers la fonction de la tache
    uint32_t stack[STACK_SIZE]; // Espace pile de la tache
} TCB_t;
```

### Organisation memoire

```
RAM (SRAM)
+---------------------------+
|  Variables globales       |  .data + .bss
+---------------------------+
|  TCB[0] + Stack Task 0    |  256 mots (1 kB)
+---------------------------+
|  TCB[1] + Stack Task 1    |  256 mots (1 kB)
+---------------------------+
|  TCB[2] + Stack Task 2    |  256 mots (1 kB)
+---------------------------+
|  Pile systeme (MSP)       |  Utilisee par les ISR
+---------------------------+
```

## 1.3 Principe de Fonctionnement

```
Temps →  |  Task1  |  Task2  |  Task3  |  Task1  |  Task2  | ...
         |← tick →|← tick →|← tick →|← tick →|← tick →|

A chaque tick SysTick :
1. Interruption SysTick declenchee
2. Sauvegarde contexte tache courante (PUSH R4-R11 sur sa pile)
3. Sauvegarde SP dans TCB[current].stack_pointer
4. current_task = (current_task + 1) % NUM_TASKS
5. Restaure SP depuis TCB[current].stack_pointer
6. Restaure contexte nouvelle tache (POP R4-R11 depuis sa pile)
7. Retour d'exception → reprend la nouvelle tache
```

## 1.4 Roles des Exceptions

| Exception | Role dans le RTOS |
|-----------|-------------------|
| **SysTick** | Declenche le changement de tache a intervalle regulier |
| **PendSV** | Execute le changement de contexte (priorite la plus basse) |
| **SVCall** | Peut etre utilise pour demarrer le premier tache |

> **Pourquoi PendSV ?** Le changement de contexte doit avoir la **priorite la plus basse** pour ne pas interrompre d'autres ISR critiques. SysTick met PendSV en attente, PendSV s'execute quand toutes les autres ISR sont terminees.

---

[Suivant : Scheduler >>](./02-Scheduler.md)
