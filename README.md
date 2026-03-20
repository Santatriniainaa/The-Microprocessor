# The Microprocessor - ARM Cortex-M0 : Cours, Projets & Codebase

> **Auteur** : Santatriniaina RAZAFINDRAKOTO
> **Plateforme** : ARM Cortex-M0 / Keil MDK
> **Livre** : *The Microprocessor: A Practical Introduction Using The ARM Cortex-M0 Processor* — Vasavada & Sinha (Wiley, 2025)

---

## Presentation

Ce depot contient un **ecosysteme complet** pour l'etude et la maitrise du microprocesseur ARM Cortex-M0 :

- **22 fiches de cours** en français avec diagrammes ASCII, tableaux et code
- **8 corriges d'exercices** avec programmes assembleur et C complets
- **6 projets professionnels** avec codebases fonctionnelles
- **1 projet final innovant** (TSAKITSAKY ENERGY) avec business plan, hardware design et 1 487 lignes de code C

```
                          ┌─────────────────────┐
                          │    ARM Cortex-M0     │
                          │  ┌─────┐  ┌──────┐  │
                          │  │NVIC │  │Debug │  │
                          │  └──┬──┘  └──────┘  │
                          │     │               │
                          │  ┌──▼──────────┐    │
                          │  │ Coeur 32-bit │    │
                          │  │ Pipeline 3et.│    │
                          │  │ Thumb ISA    │    │
                          │  └──────────────┘    │
                          └──────────┬──────────┘
                                     │ AHB-Lite
                          ┌──────────┼──────────┐
                          │          │          │
                       Flash      SRAM     Peripheriques
                       (ROM)      (RAM)    (GPIO,UART,ADC)
```

---

## Structure du Depot

```
The-Microprocessor/
│
├── README.md                              ← CE FICHIER
│
├── cours/                                 ← 22 fiches de cours (français)
│   ├── 00-INDEX.md                          Index general + carte memoire
│   ├── 00-Introduction.md                   Types de processeurs & ARM
│   ├── 01-Systeme-Microprocesseur.md        CPU, memoire, bus, E/S
│   ├── 02-Architecture-Jeu-Instructions.md  Registres, ISA, 56 instructions
│   ├── 03-Execution-Programme.md            Fetch/Decode/Execute, pipeline
│   ├── 04-Programmation-Assembleur.md       Sections, directives, labels
│   ├── 05-Operations-Arithmetiques.md       ADD/SUB/MUL, drapeaux NZCV
│   ├── 06-Operations-Niveau-Bit.md          AND/OR/XOR, decalages, masques
│   ├── 07-Acces-Memoire.md                  LDR/STR, alignement, endianness
│   ├── 08-Sous-Programmes.md                BL/BX LR, AAPCS, prologue/epilogue
│   ├── 09-Pile-Variables-Locales.md         Stack frame, PUSH/POP, SP
│   ├── 10-Modele-Exception.md               Table vecteurs, Handler/Thread mode
│   ├── 11-Interruptions-SysTick.md          NVIC, SysTick timer 24-bit
│   ├── 12-Programmation-C.md                Types, volatile, structures controle
│   ├── 13-Fonctions-C.md                    AAPCS, C↔ASM, recursivite
│   ├── 14-Tableaux-Structures.md            Arrays, structs, pointeurs
│   ├── 15-Fonctions-Avancees-C.md           Callbacks, preprocesseur, CMSIS
│   ├── 16-Processus-Compilation.md          Toolchain, linker, scatter file
│   ├── 17-Microcontroleur.md                SoC, horloge, GPIO, reset
│   ├── 18-Programmation-IO.md               GPIO, Timers, ADC, debouncing
│   ├── 19-Serie-UART.md                     Protocole UART, TX/RX, baud rate
│   └── A-Keil-uVision.md                   IDE, debugger, breakpoints
│
├── correct_answers/                       ← 8 corriges d'exercices
│   ├── 00-INDEX-CORRIGES.md                 Index des corriges
│   ├── ch05-arithmetique.md                 Ex 5.1-5.7 + somme tableau
│   ├── ch06-bits.md                         Ex 6.1-6.5 + compter bits a 1
│   ├── ch07-memoire.md                      Ex 7.1-7.6 + copier/inverser tableau
│   ├── ch08-branch.md                       Tri Bubble Sort en ASM
│   ├── ch09-stack.md                        Factorielle + GCD (Euclide)
│   ├── ch10-exceptions.md                   Timer SysTick simulation
│   └── ch11-12-c.md                         Programme C + analyse ASM generee
│
├── projects/                              ← 6 projets avec codebases complets
│   ├── 00-INDEX-PROJET.md                   Index + analyse business + matrice
│   ├── P1-Mini-RTOS/                        Mini-RTOS Round-Robin (8 docs)
│   ├── P2-Calculatrice-Scientifique/        Parser + REPL UART (6 fichiers C)
│   ├── P3-Systeme-Domotique/               FSM 5 etats + capteurs (8 fichiers C)
│   ├── P4-Protocole-Communication/          CRC-8 + ACK/NACK (6 fichiers C)
│   ├── P5-Data-Logger/                      Ring buffer + stats (8 fichiers C)
│   └── final-project/                       TSAKITSAKY ENERGY
│       ├── 00-PITCH.md                        Pitch + marche + rentabilite
│       ├── 01-Architecture.md                 Architecture systeme + FSM
│       ├── 02-Business-Plan.md                Plan d'affaires Madagascar
│       ├── 03-Hardware-Design.md              Schema electronique + BOM
│       ├── 04-Software-Architecture.md        Modules, flux, MPPT, SOC
│       ├── 05-Codebase.md                     Documentation du code
│       ├── 06-Tests-Validation.md             30+ tests + 3 scenarios
│       ├── 07-Deploiement.md                  Roadmap + regions + KPIs
│       ├── 08-BOM-Composants.md               Liste complete composants
│       └── src/                               20 fichiers C (1 487 lignes)
│           ├── config.h                         Configuration & seuils
│           ├── adc.h / adc.c                    Acquisition 4 canaux
│           ├── energy.h / energy.c              Calculs P, SOC, bilan
│           ├── fsm.h / fsm.c                    Machine a 6 etats
│           ├── load.h / load.c                  Controle 3 charges
│           ├── alert.h / alert.c                LEDs statut (R/O/V)
│           ├── mppt.h / mppt.c                  MPPT Perturb & Observe
│           ├── logger.h / logger.c              Historique ring buffer
│           ├── display.h / display.c            Affichage UART
│           ├── uart_io.h / uart_io.c            Driver UART
│           └── main.c                           Programme principal
│
└── *.pdf                                  ← Livres et instructions
    ├── The Microprocessor [...].pdf         Livre de reference (516 pages)
    └── Instructions microprocesseur II.pdf  Instructions du projet final
```

---

## Demarrage Rapide

### Prerequis

| Outil | Version | Utilisation |
|-------|---------|-------------|
| **Keil MDK** | v5.38+ (MDK-Lite gratuit) | IDE + compilateur + simulateur |
| **armclang** | Integre dans Keil MDK | Compilateur C/ASM ARM |
| **Git** | 2.x+ | Gestion de version |

### Etapes

```bash
# 1. Cloner le depot
git clone <url-du-depot>

# 2. Ouvrir Keil uVision
#    Project → New uVision Project
#    Selectionner : ARM → ARM Cortex-M0

# 3. Ajouter les fichiers source d'un projet
#    Ex: projects/final-project/src/*.c et *.h

# 4. Build (F7) puis Debug (Ctrl+F5)

# 5. Examiner les registres, memoire et sortie UART
```

### Pour les cours

Ouvrir `cours/00-INDEX.md` dans n'importe quel lecteur Markdown (VS Code, Obsidian, GitHub).

---

## Projet Final : TSAKITSAKY ENERGY

### Controleur solaire intelligent pour Madagascar

| Critere | Detail |
|---------|--------|
| **Probleme** | 85% des ruraux malgaches sans electricite |
| **Solution** | MCU Cortex-M0 qui mesure, optimise, protege et communique |
| **Codebase** | 20 fichiers C, 1 487 lignes, 8 modules |
| **Prix cible** | $45-65 USD (vs $80-200 concurrence) |
| **Marge** | 60-73% |
| **Impact A5** | 100 000 foyers electrifies |

```
☀ Panneau ──ADC──→ ┌──────────────────┐ ──GPIO──→ Eclairage (P1)
🔋 Batterie ─ADC──→ │   TSAKITSAKY     │ ──GPIO──→ Chargeur  (P2)
⚡ I_panel ──ADC──→ │   Cortex-M0      │ ──GPIO──→ Radio/TV  (P3)
⚡ I_load ───ADC──→ │   FSM + MPPT     │ ──UART──→ LCD/GSM
                    └──────────────────┘ ──LED───→ Status (R/O/V)
```

Voir [`projects/final-project/00-PITCH.md`](./projects/final-project/00-PITCH.md) pour le pitch complet.

---

## Statistiques du Depot

| Categorie | Fichiers | Lignes |
|-----------|---------|--------|
| Cours (markdown) | 22 | ~5 200 |
| Corriges (markdown) | 8 | ~1 500 |
| Projets documentation | 20 | ~4 000 |
| Code C (final-project/src) | 20 | 1 487 |
| **Total** | **~72** | **~12 000+** |

---

## Technologies et Competences

### Hardware

| Composant | Role |
|-----------|------|
| ARM Cortex-M0 | Coeur processeur 32-bit RISC |
| ADC 12-bit | Mesure tensions et courants |
| GPIO | Controle charges et LEDs |
| UART | Communication serie |
| SysTick | Timer systeme (interruptions) |
| NVIC | Controleur d'interruptions |

### Software

| Technologie | Utilisation |
|-------------|-------------|
| C (C99) | Firmware embarque |
| ARM Assembly (Thumb) | Exercices bas niveau, context switch |
| Keil MDK / armclang | Compilation et debogage |
| Git | Gestion de version |
| Markdown | Documentation |

### Concepts maitrises

- Architecture RISC et pipeline 3 etages
- Programmation assembleur ARM (Thumb ISA)
- Gestion memoire : pile, tas, alignement, endianness
- Interruptions, exceptions, NVIC, SysTick
- Machine a etats finis (FSM)
- Algorithme MPPT (Perturb & Observe)
- Protocoles de communication (UART, CRC)
- Conception de systemes embarques temps reel (RTOS)

---

## Licence

Ce depot est un travail academique et de recherche. Le contenu est base sur le livre *The Microprocessor* (Wiley, 2025) et les instructions de cours.

---

> *"Ny herin'ny masoandro ho an'ny rehetra"* — La puissance du soleil pour tous
