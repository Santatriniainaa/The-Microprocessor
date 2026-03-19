# Le Microprocesseur : Introduction Pratique avec l'ARM Cortex-M0

> **Livre de reference** : *The Microprocessor: A Practical Introduction Using The ARM Cortex-M0 Processor*
> **Auteurs** : Darshak S. Vasavada, S. K. Sinha
> **Editeur** : John Wiley & Sons, 2025

---

## Table des Matieres

### Partie I : Fondamentaux du Systeme Microprocesseur

| # | Chapitre | Fichier |
|---|---------|---------|
| 0 | [Introduction - Types de Processeurs & ARM Cortex](./00-Introduction.md) | `00-Introduction.md` |
| 1 | [Systeme Microprocesseur](./01-Systeme-Microprocesseur.md) | `01-Systeme-Microprocesseur.md` |
| 2 | [Architecture du Jeu d'Instructions](./02-Architecture-Jeu-Instructions.md) | `02-Architecture-Jeu-Instructions.md` |
| 3 | [Execution des Programmes](./03-Execution-Programme.md) | `03-Execution-Programme.md` |

### Partie II : Programmation Assembleur

| # | Chapitre | Fichier |
|---|---------|---------|
| 4 | [Programmation en Assembleur](./04-Programmation-Assembleur.md) | `04-Programmation-Assembleur.md` |
| 5 | [Operations Arithmetiques](./05-Operations-Arithmetiques.md) | `05-Operations-Arithmetiques.md` |
| 6 | [Operations au Niveau Bit](./06-Operations-Niveau-Bit.md) | `06-Operations-Niveau-Bit.md` |
| 7 | [Acces Memoire](./07-Acces-Memoire.md) | `07-Acces-Memoire.md` |
| 8 | [Sous-Programmes et Appels de Fonctions](./08-Sous-Programmes.md) | `08-Sous-Programmes.md` |
| 9 | [Pile et Variables Locales](./09-Pile-Variables-Locales.md) | `09-Pile-Variables-Locales.md` |

### Partie III : Modele Systeme

| # | Chapitre | Fichier |
|---|---------|---------|
| 10 | [Modele d'Exception](./10-Modele-Exception.md) | `10-Modele-Exception.md` |
| 11 | [Interruptions et Timer SysTick](./11-Interruptions-SysTick.md) | `11-Interruptions-SysTick.md` |

### Partie IV : Programmation en C

| # | Chapitre | Fichier |
|---|---------|---------|
| 12 | [Programmation en C](./12-Programmation-C.md) | `12-Programmation-C.md` |
| 13 | [Fonctions en C](./13-Fonctions-C.md) | `13-Fonctions-C.md` |
| 14 | [Tableaux et Structures](./14-Tableaux-Structures.md) | `14-Tableaux-Structures.md` |
| 15 | [Fonctions Avancees en C](./15-Fonctions-Avancees-C.md) | `15-Fonctions-Avancees-C.md` |
| 16 | [Processus de Compilation](./16-Processus-Compilation.md) | `16-Processus-Compilation.md` |

### Partie V : Systeme Microcontroleur

| # | Chapitre | Fichier |
|---|---------|---------|
| 17 | [Le Microcontroleur](./17-Microcontroleur.md) | `17-Microcontroleur.md` |
| 18 | [Programmation des Entrees/Sorties](./18-Programmation-IO.md) | `18-Programmation-IO.md` |
| 19 | [Communication Serie - UART](./19-Serie-UART.md) | `19-Serie-UART.md` |

### Annexes

| # | Annexe | Fichier |
|---|--------|---------|
| A | [Environnement Keil uVision](./A-Keil-uVision.md) | `A-Keil-uVision.md` |

---

## Vue d'ensemble du processeur ARM Cortex-M0

```
+--------------------------------------------------+
|              Processeur Cortex-M0                  |
|                                                    |
|  +-------+    +-------------+    +-----------+     |
|  | NVIC  |--->| Coeur       |<-->| Unite de  |     |
|  |       |    | Cortex-M0   |    | Debogage  |<--> Debugger
|  +-------+    +------+------+    +-----------+     |
|       ^              |                             |
|  Interruptions  Bus interne                        |
|                      |                             |
+----------------------|-----------------------------+
                       |
              Interface bus AHB-Lite
                       |
        +--------------+--------------+
        |              |              |
   +----+----+   +-----+-----+  +----+----+
   |  Flash  |   |   SRAM    |  | Ports   |
   |  (ROM)  |   |   (RAM)   |  |  E/S    |
   +---------+   +-----------+  +---------+
```

## Carte memoire Cortex-M0

| Plage d'adresses | Region | Utilisation |
|-----------------|--------|-------------|
| `0x00000000 - 0x1FFFFFFF` | Code | Flash/ROM (instructions + constantes) |
| `0x20000000 - 0x3FFFFFFF` | SRAM | RAM (variables, pile) |
| `0x40000000 - 0x5FFFFFFF` | Peripherique | Registres des ports E/S |
| `0x60000000 - 0x9FFFFFFF` | RAM externe | Memoire externe (si presente) |
| `0xA0000000 - 0xDFFFFFFF` | Device externe | Peripheriques externes |
| `0xE0000000 - 0xFFFFFFFF` | Systeme | NVIC, SysTick, Debug |

## Jeu d'instructions Cortex-M0 (resume)

| Categorie | Instructions |
|-----------|-------------|
| **Traitement de donnees** | `mov`, `add`, `adc`, `adr`, `sub`, `sbc`, `mul`, `cmp`, `cmn`, `neg`, `sxtb`, `sxth`, `uxtb`, `uxth`, `rev`, `rev16`, `revsh` |
| **Logique** | `and`, `orr`, `eor`, `not`, `lsl`, `lsr`, `asr`, `ror`, `bic`, `tst` |
| **Acces memoire** | `ldr`, `ldrb`, `ldrh`, `ldrsb`, `ldrsh`, `str`, `strb`, `strh`, `ldm`, `stm` |
| **Pile** | `push`, `pop` |
| **Controle programme** | `b`, `bx`, `bl`, `blx` |
| **Systeme** | `bkpt`, `cps`, `mrs`, `msr`, `nop`, `svc`, `dmb`, `dsb`, `isb`, `sev`, `wfe`, `wfi`, `yield` |

---

## Registres du Cortex-M0

```
+--------+--------+
| R0     | R8     |     R0-R7  : Registres bas (usage general)
| R1     | R9     |     R8-R12 : Registres hauts (usage limite)
| R2     | R10    |     R13/SP : Pointeur de pile
| R3     | R11    |     R14/LR : Registre de lien
| R4     | R12    |     R15/PC : Compteur de programme
| R5     | R13=SP |
| R6     | R14=LR |     APSR: [N|Z|C|V|...|0]
| R7     | R15=PC |       N=Negatif Z=Zero C=Carry V=Overflow
+--------+--------+
```

---

> **Note** : Ces fichiers constituent un support d'etude complet base sur le livre. Ils sont organises pour une progression pedagogique logique, du materiel (hardware) vers le logiciel (software), de l'assembleur vers le C.
