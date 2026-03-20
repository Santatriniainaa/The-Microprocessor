# Chapitre 10 : Modele d'Exception

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 9](./09-Pile-Variables-Locales.md) | [Chapitre 11 >>](./11-Interruptions-SysTick.md)

---

## Objectifs

> Comprendre le modele d'exception du Cortex-M0 : table des vecteurs, modes du processeur, entree/sortie d'exception, et gestion des priorites.

---

## 10.1 Qu'est-ce qu'une Exception ?

Une exception est un **evenement** qui interrompt le flux normal du programme. Le processeur sauvegarde son contexte et execute un **handler** (routine de traitement).

### Types d'exceptions

| Type | Exemples |
|------|----------|
| **Reset** | Mise sous tension, bouton reset |
| **Fautes** | HardFault (instruction invalide, acces non aligne) |
| **Interruptions systeme** | SVCall, PendSV, SysTick |
| **Interruptions externes** | IRQ des peripheriques (GPIO, Timer, UART...) |

---

## 10.2 Table des Vecteurs

Situee a l'adresse **0x00000000**, elle contient les adresses des handlers :

| # | Offset | Exception | Priorite | Description |
|---|--------|-----------|----------|-------------|
| - | 0x0000 | - | - | Valeur initiale du SP |
| 1 | 0x0004 | **Reset** | **-3** (fixe) | Demarrage du systeme |
| 2 | 0x0008 | **NMI** | **-2** (fixe) | Interruption non masquable |
| 3 | 0x000C | **HardFault** | **-1** (fixe) | Faute materielle |
| 4-10 | 0x0010-0x0028 | Reserve | - | Non utilise sur M0 |
| 11 | 0x002C | **SVCall** | Configurable | Appel systeme (SVC) |
| 12-13 | 0x0030-0x0034 | Reserve | - | Non utilise |
| 14 | 0x0038 | **PendSV** | Configurable | Pending service call |
| 15 | 0x003C | **SysTick** | Configurable | Timer systeme |
| 16 | 0x0040 | **IRQ0** | Configurable | 1ere interruption externe |
| 17 | 0x0044 | **IRQ1** | Configurable | 2eme interruption externe |
| ... | ... | ... | ... | ... |
| 47 | 0x00BC | **IRQ31** | Configurable | 32eme interruption externe |

```
Adresse 0x00000000 :
+------------------+
| SP initial       |  ← Valeur chargee dans SP au reset
+------------------+
| Reset_Handler    |  ← Adresse du handler de reset
+------------------+
| NMI_Handler      |
+------------------+
| HardFault_Handler|
+------------------+
| ...              |
+------------------+
| SysTick_Handler  |
+------------------+
| IRQ0_Handler     |
+------------------+
| IRQ1_Handler     |
+------------------+
| ...              |
+------------------+
```

---

## 10.3 Registres d'Etat du Processeur

### PSR (Program Status Register)

Le PSR combine trois sous-registres :

```
Bit:  31 30 29 28 27  ...  24  ...  8 7 6 5 4 3 2 1 0
      N  Z  C  V       T               Exception Number
      ├── APSR ──┤     ├EPSR┤          ├──── IPSR ─────┤
```

| Sous-registre | Bits | Contenu |
|--------------|------|---------|
| **APSR** | 31-28 | Drapeaux N, Z, C, V |
| **IPSR** | 5-0 | Numero de l'exception en cours (0 = Thread mode) |
| **EPSR** | 24 | Bit T (Thumb) - toujours 1 sur Cortex-M |

### PRIMASK

| Bit | Fonction |
|-----|----------|
| 0 | 1 = **toutes** les interruptions masquees (sauf NMI et HardFault) |

```asm
cpsid i     // PRIMASK = 1 → desactiver les interruptions
cpsie i     // PRIMASK = 0 → activer les interruptions
```

### CONTROL

| Bit | Fonction |
|-----|----------|
| 1 | Selection pile : 0 = MSP, 1 = PSP |
| 0 | 0 = Privilege, 1 = Non-privilege (pas de MPU sur M0) |

---

## 10.4 Modes du Processeur

```
+-------------------+       Exception        +-------------------+
|   Thread Mode     | ──────────────────────→ |   Handler Mode    |
| (programme normal)|                         | (handler except.) |
|                   | ←────────────────────── |                   |
| Pile : MSP ou PSP |     Retour exception    | Pile : MSP        |
+-------------------+                         +-------------------+
```

| Mode | Quand | Pile utilisee |
|------|-------|---------------|
| **Thread** | Execution normale du programme | MSP ou PSP |
| **Handler** | Pendant le traitement d'une exception | Toujours **MSP** |

---

## 10.5 Entree en Exception (Exception Entry)

Quand une exception est declenchee, le processeur effectue **automatiquement** :

### 1. Sauvegarde du contexte (8 registres sur la pile)

```
Adresses hautes (pile avant exception)
+----------+
| xPSR     |  ← SP avant - 4
+----------+
| PC        |  ← SP avant - 8  (adresse de retour)
+----------+
| LR        |  ← SP avant - 12
+----------+
| R12       |  ← SP avant - 16
+----------+
| R3        |  ← SP avant - 20
+----------+
| R2        |  ← SP avant - 24
+----------+
| R1        |  ← SP avant - 28
+----------+
| R0        |  ← SP avant - 32 = nouveau SP
+----------+
```

### 2. Chargement du handler

- PC ← adresse du handler (depuis la table des vecteurs)
- Passage en **Handler mode**
- LR ← valeur speciale **EXC_RETURN**

### 3. EXC_RETURN

| Valeur | Signification |
|--------|---------------|
| `0xFFFFFFF1` | Retour en Handler mode, utiliser MSP |
| `0xFFFFFFF9` | Retour en Thread mode, utiliser MSP |
| `0xFFFFFFFD` | Retour en Thread mode, utiliser PSP |

---

## 10.6 Retour d'Exception

L'instruction `BX LR` avec la valeur EXC_RETURN declenche :

1. Restauration automatique des 8 registres depuis la pile
2. Retour au mode Thread (ou Handler si imbrique)
3. Reprise de l'execution a l'adresse sauvegardee dans PC

```asm
// Handler d'interruption typique
IRQ_Handler:
    // Le contexte est deja sauvegarde automatiquement
    // ... traitement de l'interruption ...
    // Acquitter l'interruption (clear pending flag)
    bx lr          // Retour automatique (EXC_RETURN dans LR)
```

---

## 10.7 HardFault

Le HardFault est declenche par :
- Acces memoire **non aligne** (mot sur adresse impaire)
- Acces a une adresse **non mappee** (bus fault)
- Instruction **indefinie**
- Division par zero (sur Cortex-M3+)
- Execution depuis une region non executable

> **Priorite -1** : Seuls Reset (-3) et NMI (-2) sont plus prioritaires.

### Debugging d'un HardFault

1. Examiner le PC sauvegarde sur la pile pour trouver l'instruction fautive
2. Verifier l'alignement des acces memoire
3. Verifier que les adresses sont dans des plages mappees

---

## 10.8 Priorite et Preemption

### Niveaux de priorite sur Cortex-M0

- **2 bits** de priorite configurable → **4 niveaux** (0, 1, 2, 3)
- **0 = priorite la plus haute**
- Reset (-3), NMI (-2), HardFault (-1) : priorites fixes, toujours superieures

### Preemption

Une exception de **priorite superieure** (valeur numerique plus basse) peut **interrompre** un handler en cours d'execution.

```
Thread mode
  └── IRQ_A (priorite 2) commence
       └── IRQ_B (priorite 0) preempte IRQ_A
            └── IRQ_B termine, retour a IRQ_A
                 └── IRQ_A termine, retour au Thread mode
```

### Tail-Chaining

Si une interruption est en attente quand un handler se termine, le processeur **enchaine** directement sans restaurer/sauvegarder le contexte → gain de temps.

---

## Resume

- **Table des vecteurs** : SP initial + adresses des handlers a 0x00000000
- **Thread mode** (normal) vs **Handler mode** (exception)
- **Entree en exception** : sauvegarde auto de 8 registres, PC ← handler
- **EXC_RETURN** dans LR, `BX LR` pour retourner
- **HardFault** : priorite -1, declenchee par erreurs d'acces/instructions
- **Cortex-M0** : 4 niveaux de priorite (0-3), preemption possible

---

[<< Chapitre 9](./09-Pile-Variables-Locales.md) | [Chapitre 11 : Interruptions et SysTick >>](./11-Interruptions-SysTick.md)
