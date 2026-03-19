# Chapitre 3 : Execution des Programmes

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 2](./02-Architecture-Jeu-Instructions.md) | [Chapitre 4 >>](./04-Programmation-Assembleur.md)

---

## Objectifs

> Comprendre comment un programme s'execute a l'interieur du processeur : les etapes fetch, decode et execute, ainsi que le pipeline.

---

## 3.1 Introduction : RISC vs CISC

| Critere | RISC | CISC |
|---------|------|------|
| Instructions | Simples, taille fixe | Complexes, taille variable |
| Cycles/instruction | Generalement 1 | Variable (1 a plusieurs) |
| Pipeline | Tres efficace | Plus complexe |
| Exemples | **ARM**, RISC-V, MIPS | x86, x86-64 |

> Le **Cortex-M0** est un processeur **RISC** base sur l'architecture **load-store** : toutes les operations se font sur les registres. Les donnees doivent d'abord etre chargees (load) depuis la memoire vers les registres, puis les resultats sont stockes (store) en memoire.

### Architecture Von Neumann vs Harvard

| Architecture | Bus | Utilise par |
|-------------|-----|-------------|
| **Von Neumann** | Bus unique pour code et donnees | **ARMv6-M** (Cortex-M0) |
| **Harvard** | Bus separes code/donnees | ARMv7-M (Cortex-M3/M4/M7) |

---

## 3.2 Programme

Un programme = **codes d'instructions** + **donnees**, stockes en memoire sous forme binaire.

### Exemple : y = m * x + c

```
Adresse   Code    Assembleur          Description
0x200     0x6803  ldr r3, [r0]        Charger x dans r3
0x202     0x2207  movs r2, #7         m = 7 dans r2
0x204     0x4353  muls r3, r2, r3     r3 = m * x
0x206     0x330B  adds r3, #11        r3 = m*x + c (c=11)
0x208     0x600B  str r3, [r1]        Stocker resultat dans y

Donnees :
0x20000000  0x0000000A    x = 10
0x20000004  0x00000000    y = 0 (resultat)
```

---

## 3.3 Interieur du CPU

Le CPU RISC simplifie comprend **trois unites** :

```
+---------------------------------------------------------------------+
|                         Processeur                                   |
|                                                                      |
|  +----------------+    +-------------+                               |
|  |  Unite de      |    | Unite de    |     Acces          Memoire    |
|  |  Decodage      |<---| Fetch       |<===========+   +----------+  |
|  |                |    | +--------+  |  Bus instr. |   | Codes    |  |
|  | Signaux de     |    | |   PC   |  |             +-->| instruct.|  |
|  | controle       |    | +--------+  |                 +----------+  |
|  |  op  src  dst  |    | | Buffer |  |                               |
|  +---|---|---|-----+    +-------------+                               |
|      |   |   |                                     Acces             |
|  +---v---v---v-----+                    +=========+   +----------+  |
|  |  Unite           |  Bus donnees      |          |   | Donnees  |  |
|  |  d'Execution     |<=================>+--------->+-->|          |  |
|  | +------+ +-----+ |                               +----------+  |
|  | | ALU  | | LSU | |                                               |
|  | +------+ +-----+ |                                               |
|  | | Banc registres| |                                               |
|  +------------------+                                               |
+---------------------------------------------------------------------+
```

---

## 3.4 Unite de Fetch (Recherche)

L'unite de fetch **lit les codes d'instructions** depuis la memoire.

**Mecanisme :**
1. Place l'adresse du **PC** sur le bus d'adresses
2. Lit le code instruction via le bus de donnees
3. Stocke dans le **tampon d'instruction**
4. **Incremente le PC** pour la prochaine instruction

> **Cortex-M0** : Bus de donnees 32 bits, instructions 16 bits → **2 instructions par fetch** en un seul cycle.

### Exemple de fetch

```
PC = 0x200
1. CPU met 0x200 sur le bus d'adresses
2. Memoire repond avec 32 bits : 0x6803 | 0x2207 (2 instructions)
3. Stocke dans le buffer
4. PC ← 0x204
```

---

## 3.5 Unite de Decodage

Traduit le code instruction en **signaux de controle** :

| Signal | Role | Exemple |
|--------|------|---------|
| **op** | Operation a effectuer | ADD, MUL, LDR, STR... |
| **src** | Source des operandes | Registres Rn, Rm ou immediat |
| **dst** | Destination du resultat | Registre Rd |

### 3.5.1 Decodage instructions traitement de donnees

```
+--------+-----+-----+------+
| Opcode |  Rd |  Rn | Rm/Imm|     → op = operation
+--------+-----+-----+------+       src = Rn, Rm/Imm
                                     dst = Rd
```

### 3.5.2 Decodage instructions load-store

```
+--------+-----+------+-----+
| Opcode |  Rn | Rm/Imm| Rt |     → op = LDR ou STR
+--------+-----+------+-----+       src = Rn + offset
                                     dst = Rt (pour load)
```

### Decodage instructions de controle

- **dst** pointe toujours vers le **PC**
- **src** = adresse cible (immediat ou registre) + drapeaux APSR (pour conditionnel)
- Pour BL/BLX : dst inclut aussi **LR**

---

## 3.6 Unite d'Execution

```
     src                dst               op
      |                  |                 |
+-----v------------------v-----------------v-----+
|                 Unite d'Execution                |
|                                                  |
|  +-----------+                                   |
|  |  Banc de  |  Rn ──→ op1                       |
|  | Registres |  Rm ──→ op2    +-------+          |
|  |  R0..R15  |  Rt ──────────→|       |          |
|  |           |         ┌─────→| ALU   |→ Result  |
|  |     Rd ←──────────┐ |     +-------+    |      |
|  +-----------+       | |       ↑          |      |
|                      | |    APSR ←────────┘      |
|                      | |                         |
|                      | | Base+Offset  Rt         |
|                      | |    ↓         ↓          |
|                      | |  +-------+ +---+  Bus   |
|                      | └──| LSU   |←|   |←====→ Memoire
|                      └────|       |→| Rt|        |
|                           +-------+ +---+        |
+--------------------------------------------------+
```

### 3.6.1 Banc de registres
- R0-R15, tous 32 bits
- Fournit les **entrees** a l'ALU et la LSU
- Reçoit les **sorties** (resultat)

### 3.6.2 ALU (Unite Arithmetique et Logique)

```
    op1       op2       (depuis Fetch unit)
     ↓         ↓              ↓
  +--+--+--+--+--+--+     signal op
  | add|sub|mul|and|...|←────┘
  +--+--+--+--+--+--+
           ↓
        Result
           ↓
        APSR ← (N, Z, C, V mis a jour)
```

- Reçoit 2 operandes, effectue l'operation selectionnee par `op`
- Met a jour les **drapeaux APSR** si specifie

### 3.6.3 LSU (Unite Load-Store)

```
  Base   Offset    Rt (donnee a stocker)
   ↓       ↓         ↓
+------+---------+--------+
| mem  |  mem    |        |     ← signal op (read/write)
| read | write   |        |
+------+---------+--------+
         ↕
    Bus ←→ Memoire
         ↓
    Rt (donnee chargee)
```

---

## 3.7 Execution Complete d'une Instruction

### Exemple : Instruction arithmetique

```asm
MULS R3, R2, R3    ; R3 ← R2 * R3    Code: 0x4353
```

```
1. FETCH  : Lit 0x4353 depuis l'adresse 0x204
2. DECODE : op = MUL, src = R2 et R3, dst = R3
3. EXECUTE: ALU multiplie R2 × R3, resultat → R3, APSR mis a jour
```

### Exemple : Instruction load

```asm
LDR R3, [R0]       ; R3 ← Memoire[R0]    Code: 0x6803
```

```
1. FETCH  : Lit 0x6803 depuis l'adresse 0x200
2. DECODE : op = LDR (mem read), src = R0 + offset 0, dst = R3
3. EXECUTE: LSU lit la memoire a l'adresse R0, donnee → R3
```

> **Analogie** : L'execution dans le processeur est comme des **marionnettes a fils**. Les signaux de controle (op, src, dst) sont les fils. L'unite de decodage est le marionnettiste. L'unite d'execution est la marionnette. L'intelligence est dans le **programme** (les codes d'instructions), pas dans le materiel !

---

## 3.8 Pipeline du Processeur

### Execution sequentielle (sans pipeline)

```
Temps →   1   2   3   4   5   6   7   8   9  10  11  12
         +---+---+---+---+---+---+---+---+---+---+---+---+
Instr 1  | F | D | E |   |   |   |   |   |   |   |   |   |
Instr 2  |   |   |   | F | D | E |   |   |   |   |   |   |
Instr 3  |   |   |   |   |   |   | F | D | E |   |   |   |
Instr 4  |   |   |   |   |   |   |   |   |   | F | D | E |
         +---+---+---+---+---+---+---+---+---+---+---+---+

→ 4 instructions en 12 cycles (3N cycles pour N instructions)
```

### Execution pipelinee (avec pipeline)

```
Temps →   1   2   3   4   5   6
         +---+---+---+---+---+---+
Instr 1  | F | D | E |   |   |   |
Instr 2  |   | F | D | E |   |   |
Instr 3  |   |   | F | D | E |   |
Instr 4  |   |   |   | F | D | E |
         +---+---+---+---+---+---+

→ 4 instructions en 6 cycles (N+2 cycles pour N instructions)
```

### Pipeline 3 etages du Cortex-M0

```
Instructions  →  [Instr 7] [Instr 6] [Instr 5] [Instr 4]
                      ↓         ↓         ↓
                  +--------+--------+--------+
                  | FETCH  | DECODE | EXECUTE|  → Instr 1 terminee
                  +--------+--------+--------+

Apres la 1ere instruction, une instruction se termine
a CHAQUE cycle d'horloge !
```

### Pipelines de differents processeurs

| Processeur | Etages | Frequence max |
|-----------|--------|---------------|
| **Cortex-M0/M3/M4** | 3 | ~200 MHz |
| **Cortex-M7** | 6 | ~400 MHz |
| **Cortex-R4** | 8 | ~600 MHz |
| **Cortex-R7** | 11 | ~1 GHz |

> Plus d'etages = frequence plus elevee mais complexite accrue (aleas de pipeline, prediction de branchement).

---

## 3.9 Resume

### Programme
- Un programme = codes d'instructions + donnees en binaire en memoire

### Execution d'instruction
- 3 etapes : **Fetch** (recherche) → **Decode** (decodage) → **Execute** (execution)
- Fetch : lit le code instruction a l'adresse du PC
- Decode : traduit en signaux op, src, dst
- Execute : ALU (arithmetique/logique) ou LSU (load/store)

### Unite d'execution
- **Banc de registres** : R0-R15, fournit operandes et reçoit resultats
- **ALU** : operations arithmetiques et logiques, met a jour APSR
- **LSU** : operations de lecture/ecriture memoire

### Pipeline
- Chevauchement des etapes F/D/E pour N instructions
- Sans pipeline : 3N cycles | Avec pipeline : N+2 cycles
- Cortex-M0 : pipeline **3 etages**

---

[<< Chapitre 2](./02-Architecture-Jeu-Instructions.md) | [Chapitre 4 : Programmation Assembleur >>](./04-Programmation-Assembleur.md)
