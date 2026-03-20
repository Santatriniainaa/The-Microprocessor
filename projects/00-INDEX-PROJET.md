# Portfolio de Projets ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO
> **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0 / Keil MDK (Simulation)

---

## Vue d'Ensemble des Projets

```
 P1 Mini-RTOS          P2 Calculatrice        P3 Domotique
 ┌─────────────┐       ┌─────────────┐       ┌─────────────┐
 │ Scheduler   │       │ Parser      │       │ Capteurs    │
 │ Context SW  │       │ ALU 32-bit  │       │ Actionneurs │
 │ SysTick     │       │ Stack eval  │       │ Machine FSM │
 │ 3+ taches   │       │ UART I/O    │       │ Alertes     │
 └─────────────┘       └─────────────┘       └─────────────┘
   Ch.9-11               Ch.5-8                Ch.17-19

 P4 Protocole           P5 Data Logger
 ┌─────────────┐       ┌─────────────┐
 │ UART framing│       │ Ring Buffer │
 │ CRC-8       │       │ ADC sampling│
 │ ACK/NACK    │       │ Timestamps  │
 │ Retransmit  │       │ Stats calc  │
 └─────────────┘       └─────────────┘
   Ch.6,8,19             Ch.5-7,11,18
```

---

## Catalogue des Projets

| # | Projet | Difficulte | Chapitres | Valeur Business |
|---|--------|-----------|-----------|----------------|
| **P1** | [Mini-RTOS](./P1-Mini-RTOS/) | Avance | 9-11 | Fondation pour tout systeme embarque temps reel |
| **P2** | [Calculatrice Scientifique](./P2-Calculatrice-Scientifique/) | Intermediaire | 5-8 | Traitement de donnees embarque, validation d'algorithmes |
| **P3** | [Systeme Domotique](./P3-Systeme-Domotique/) | Intermediaire | 17-19 | IoT, maison intelligente, controle industriel |
| **P4** | [Protocole de Communication](./P4-Protocole-Communication/) | Avance | 6,8,19 | Fiabilite des transmissions, industrie, automobile |
| **P5** | [Data Logger Embarque](./P5-Data-Logger/) | Intermediaire | 5-7,11,18 | Monitoring industriel, agriculture, environnement |

---

## Analyse Business (ROI par projet)

### Marche cible et valeur

| Projet | Secteur | Marche mondial | Competences acquises |
|--------|---------|---------------|---------------------|
| Mini-RTOS | Embarque generaliste | $8.2B (RTOS market 2025) | Multitache, scheduling, OS design |
| Calculatrice | Instrumentation | $52B (test & mesure) | Parsing, algorithms, UI embarquee |
| Domotique | IoT / Smart Home | $135B (smart home 2025) | Capteurs, actionneurs, FSM |
| Protocole | Telecom / Auto | $180B (automotive embedded) | Protocoles, fiabilite, CRC |
| Data Logger | Industrie / Agri | $2.5B (data loggers 2025) | Acquisition, stockage, statistiques |

### Matrice de Competences

```
                  ASM  │  C  │ IRQ │ Timer│ UART│ ADC │ Pile │ RTOS
                 ──────┼─────┼─────┼──────┼─────┼─────┼──────┼─────
P1 Mini-RTOS       ██  │ ██  │ ███ │ ███  │     │     │ ███  │ ███
P2 Calculatrice    ███ │ ██  │     │      │ ██  │     │ ██   │
P3 Domotique       █   │ ███ │ ██  │ ██   │ ██  │ ██  │      │
P4 Protocole       ██  │ ███ │ ██  │ █    │ ███ │     │      │
P5 Data Logger     █   │ ███ │ ██  │ ███  │ ██  │ ███ │      │
```

---

## Organisation du Code (Standards Industriels)

Chaque projet suit les conventions :
- **Nommage** : snake_case pour fonctions/variables, UPPER_CASE pour macros/constantes
- **Headers** : include guards, documentation Doxygen
- **Modularite** : un module = un .c + un .h
- **volatile** : obligatoire pour registres hardware et variables ISR
- **const** : donnees en ROM quand possible
- **Erreurs** : codes de retour ou enum d'erreurs

---

> **Recommandation** : Commencer par **P2** ou **P5** (intermediaire), puis **P4** ou **P3**, et terminer par **P1** (projet final du cours).
