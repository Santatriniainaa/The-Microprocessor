# P1 - Mini-RTOS sur ARM Cortex-M0 (Projet Final du Cours)

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Difficulte** : Avance | **Chapitres** : 9-11
> **Simulation** : Keil MDK Cortex-M0 Simulator
> **Statut** : PROJET FINAL OBLIGATOIRE

---

## 1. Objectif Business

Le marche des **RTOS embarques** represente **$8.2 milliards** en 2025. Ce projet developpe les competences fondamentales pour travailler avec FreeRTOS, Zephyr, RT-Thread — utilises dans l'automobile, l'aeronautique, le medical et l'IoT industriel.

## 2. Architecture et Documentation detaillee

| Document | Contenu |
|----------|---------|
| [01-Architecture-Systeme.md](./01-Architecture-Systeme.md) | TCB, organisation memoire, diagramme systeme |
| [02-Scheduler.md](./02-Scheduler.md) | Round-Robin, initialisation des piles, table des taches |
| [03-Context-Switch.md](./03-Context-Switch.md) | PendSV handler en ASM, sauvegarde/restauration R4-R11 |
| [04-SysTick-Timer.md](./04-SysTick-Timer.md) | Configuration SysTick, calcul du time slice |
| [05-Implementation.md](./05-Implementation.md) | Code source complet (main.c, scheduler.c/h, tasks.c/h) |
| [06-Rapport-Projet.md](./06-Rapport-Projet.md) | Rapport 4-6 pages |

## 3. Codebase (resume)

```
P1-Mini-RTOS/
├── README.md                    ← Ce fichier
├── 01-Architecture-Systeme.md
├── 02-Scheduler.md
├── 03-Context-Switch.md
├── 04-SysTick-Timer.md
├── 05-Implementation.md         ← Code complet : scheduler.h/c, tasks.h/c, main.c
└── 06-Rapport-Projet.md
```

> Voir [05-Implementation.md](./05-Implementation.md) pour le code source complet.

---

[Retour a l'index](../00-INDEX-PROJET.md)
