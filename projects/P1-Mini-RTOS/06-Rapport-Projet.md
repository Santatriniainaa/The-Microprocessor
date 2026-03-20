# Rapport de Projet : Mini-RTOS sur ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO
> **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0 / Keil MDK (Simulation)

---

## 1. Introduction

Ce rapport presente l'implementation d'un **mini-RTOS** (Real-Time Operating System) simplifie pour le processeur ARM Cortex-M0, execute en simulation sur **Keil MDK**. Le projet met en pratique les concepts fondamentaux etudies en cours : architecture ARM, programmation bas niveau, gestion de la pile, interruptions et ordonnancement de taches.

### Objectifs
- Gerer 3 taches independantes
- Implementer un ordonnancement **Round-Robin**
- Utiliser le timer **SysTick** pour le declenchement periodique
- Realiser un **changement de contexte** complet

---

## 2. Architecture du Systeme

### 2.1 Vue d'ensemble

Le mini-RTOS se compose de quatre modules principaux :

```
+------------------+     +------------------+
|    main.c        |     |    tasks.c       |
| (initialisation) |     | (3 taches user)  |
+--------+---------+     +--------+---------+
         |                        |
+--------v------------------------v---------+
|              scheduler.c                   |
|  - Initialisation des TCB                  |
|  - Ordonnanceur Round-Robin                |
|  - Configuration SysTick                   |
+--------------------+----------------------+
                     |
+--------------------v----------------------+
|         PendSV_Handler (ASM)               |
|  - Sauvegarde contexte (R4-R11)            |
|  - Sélection tache suivante                |
|  - Restauration contexte                   |
+-------------------------------------------+
```

### 2.2 TCB (Task Control Block)

Chaque tache possede un TCB contenant son **pointeur de pile** et son **espace pile** (256 mots = 1 kB). La pile est pre-initialisee avec un cadre d'exception simulant un retour normal.

---

## 3. Fonctionnement du Scheduler

### 3.1 Algorithme Round-Robin

L'ordonnanceur suit un cycle simple : `Task0 → Task1 → Task2 → Task0 → ...`

La selection se fait par : `current_task = (current_task + 1) % NUM_TASKS`

### 3.2 Initialisation

Avant le demarrage, chaque pile de tache est pre-remplie avec :
- **Cadre hardware** : xPSR, PC, LR, R12, R3-R0 (simule la sauvegarde automatique)
- **Cadre software** : R4-R11 (valeurs initiales a 0)

Le PC est initialise avec l'adresse de la fonction de la tache.

---

## 4. Gestion des Interruptions

### 4.1 SysTick

Le timer SysTick est configure pour generer une interruption toutes les **1 ms** (12 000 cycles a 12 MHz). Le handler SysTick met PendSV en attente.

### 4.2 PendSV

PendSV est configure a la **priorite la plus basse** (3 sur Cortex-M0) pour garantir que le changement de contexte ne preempte pas d'autres ISR critiques.

### 4.3 Flux d'interruptions

```
Tick SysTick → SysTick_Handler → Set PendSV pending
                                        |
Quand aucune autre ISR active → PendSV_Handler → Context Switch
```

---

## 5. Mecanisme de Changement de Contexte

### 5.1 Sauvegarde

1. Le CPU sauvegarde **automatiquement** R0-R3, R12, LR, PC, xPSR sur la pile PSP
2. Le handler PendSV sauvegarde **manuellement** R4-R11
3. Le SP courant est stocke dans `tcb[current].sp`

### 5.2 Restauration

1. Le SP de la nouvelle tache est charge depuis `tcb[next].sp`
2. R4-R11 sont restaures manuellement
3. PSP est mis a jour
4. `BX LR` avec EXC_RETURN = 0xFFFFFFFD declenche la restauration automatique de R0-R3, R12, LR, PC, xPSR

### 5.3 Contrainte Cortex-M0

Sur Cortex-M0, les instructions STM/LDM ne supportent que R0-R7. Pour sauvegarder R8-R11, il faut les copier d'abord dans R4-R7 temporairement.

---

## 6. Resultats et Verification

### Tests effectues dans le simulateur Keil

| Test | Resultat attendu | Statut |
|------|-----------------|--------|
| `task0_counter` incremente | Augmente a chaque execution de Task0 | [A verifier] |
| `task1_result` augmente | Somme cumulative 0+1+2+...+100 | [A verifier] |
| `task2_output` alterne | 0x00 ↔ 0xFF | [A verifier] |
| `current_task` cycle | 0 → 1 → 2 → 0 → ... | [A verifier] |
| SP change entre les taches | Adresses differentes pour chaque tache | [A verifier] |

---

## 7. Conclusion

Ce projet a permis de comprendre les mecanismes fondamentaux d'un RTOS :
- La **gestion des taches** via des TCB et des piles separees
- L'**ordonnancement Round-Robin** pour le partage equitable du CPU
- Le role des **interruptions** (SysTick + PendSV) dans le declenchement
- Le **changement de contexte** comme element central du multitache

Ces mecanismes sont a la base de RTOS professionnels comme **FreeRTOS**, **Zephyr** ou **RT-Thread**, largement utilises dans l'industrie embarquee.

---

## References

1. Vasavada, D. S., Sinha, S. K. *The Microprocessor: A Practical Introduction Using The ARM Cortex-M0 Processor*. Wiley, 2025.
2. ARM Ltd. *ARMv6-M Architecture Reference Manual*.
3. ARM Ltd. *Cortex-M0 Technical Reference Manual*.
