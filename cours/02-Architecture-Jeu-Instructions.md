# Chapitre 2 : Architecture du Jeu d'Instructions (ISA)

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 1](./01-Systeme-Microprocesseur.md) | [Chapitre 3 >>](./03-Execution-Programme.md)

---

## Objectifs

> Introduire l'architecture du jeu d'instructions du Cortex-M0 : registres, types d'instructions, structure et encodage des instructions.

---

## 2.1 Introduction

Il y a deux façons d'etudier un processeur :
- **Vue implementation** : comment le processeur est construit (diagramme bloc, materiel)
- **Vue programmeur** (Modele du programmeur) : comment utiliser le processeur (registres, instructions)

Le **modele du programmeur** a deux aspects :

| Aspect | Contenu | Chapitres |
|--------|---------|-----------|
| **Application** | Types de donnees, ISA, instructions | Ch. 2-9 |
| **Systeme** | Modes processeur, exceptions, NVIC, SysTick | Ch. 10-11 |

> **Modele memoire** : Le Cortex-M0 suit le modele memoire ARMv6-M (carte d'adresses, types 8/16/32 bits, alignement, endianness).
>
> **Modele E/S** : Non defini par ARM - chaque fabricant definit ses propres peripheriques.

---

## 2.2 Registres

Les registres sont des emplacements memoire **ultra-rapides** a l'interieur du processeur.

### Banc de registres du Cortex-M0

```
31                              0    31                              0
+--------------------------------+   +--------------------------------+
|              R0                 |   |              R8                 |
+--------------------------------+   +--------------------------------+
|              R1                 |   |              R9                 |
+--------------------------------+   +--------------------------------+
|              R2                 |   |              R10                |
+--------------------------------+   +--------------------------------+
|              R3                 |   |              R11                |
+--------------------------------+   +--------------------------------+
|              R4                 |   |              R12                |
+--------------------------------+   +--------------------------------+
|              R5                 |   |         R13 = SP               |
+--------------------------------+   +--------------------------------+
|              R6                 |   |         R14 = LR               |
+--------------------------------+   +--------------------------------+
|              R7                 |   |         R15 = PC               |
+--------------------------------+   +--------------------------------+

        Registres BAS                       Registres HAUTS
      (usage general)                    (usage limite/special)

+--------------------------------+
| Application Program Status Reg |    APSR
| N | Z | C | V |    (inutilise) |    bits 31-28 : drapeaux
+--------------------------------+
 31  30  29  28  27            0
```

### Registres a usage general : R0-R12

| Registres | Nom | Utilisation |
|-----------|-----|-------------|
| **R0 - R7** | Registres bas | Utilises par **toutes** les instructions |
| **R8 - R12** | Registres hauts | Usage **limite** sur Cortex-M0 (compatibilite) |

> **Cortex-M0 utilise principalement R0-R7.** Les registres R8-R12 existent pour la compatibilite avec les processeurs Cortex-M superieurs.

### Registres speciaux : R13, R14, R15

| Registre | Alias | Fonction |
|----------|-------|----------|
| **R13** | **SP** (Stack Pointer) | Pointeur de pile - pointe le sommet de la pile |
| **R14** | **LR** (Link Register) | Registre de lien - sauvegarde l'adresse de retour lors d'un appel de fonction |
| **R15** | **PC** (Program Counter) | Compteur de programme - adresse de la prochaine instruction a recuperer |

### Registre APSR (Application Program Status Register)

| Bit | Drapeau | Signification |
|-----|---------|---------------|
| 31 | **N** (Negative) | 1 si le resultat est negatif (bit 31 = 1) |
| 30 | **Z** (Zero) | 1 si le resultat est zero |
| 29 | **C** (Carry) | 1 si retenue/emprunt (addition/soustraction non signee) |
| 28 | **V** (oVerflow) | 1 si debordement (arithmetique signee) |

> Les drapeaux APSR sont utilises par les instructions de **branchement conditionnel** pour prendre des decisions.

### Autres registres speciaux (niveau systeme)

| Registre | Fonction |
|----------|----------|
| **PSR** | Program Status Register (contient APSR + IPSR + EPSR) |
| **PRIMASK** | Masque global des interruptions |
| **CONTROL** | Selection de la pile (MSP/PSP), mode privilege |

---

## 2.3 Jeu d'Instructions

Le Cortex-M0 possede **56 instructions uniques** classees en :

```
                    Instructions
                        |
          +-------------+-------------+
          |             |             |
   Traitement      Acces         Controle de
   de donnees     memoire        programme
      |               |              |
  +---+---+      +----+----+    +----+----+
  |       |      |         |    |         |
Arith.  Logique Load-   Pile  Branch.  Sous-
              Store              programme
```

### Table complete des instructions Cortex-M0

| Categorie | Instructions |
|-----------|-------------|
| **Arithmetique** | `mov`/`cpy`, `add`, `adc`, `adr`, `sub`, `sbc`, `mul`, `cmp`, `cmn`, `neg`/`rsb`, `sxtb`, `sxth`, `uxtb`, `uxth`, `rev`, `rev16`, `revsh` |
| **Logique** | `and`, `orr`, `eor`, `not`/`mvn`, `lsl`, `lsr`, `asr`, `ror`, `bic`, `tst` |
| **Load-Store** | `ldr`, `ldrb`, `ldrh`, `ldrsb`, `ldrsh`, `str`, `strb`, `strh` |
| **Multiple** | `ldm`/`ldmia`, `stm`/`stmia` |
| **Pile** | `push`, `pop` |
| **Branchement** | `b`, `bx`, `bl`, `blx` |
| **Systeme** | `bkpt`, `cps`, `mrs`, `msr`, `nop`, `svc`, `dmb`, `dsb`, `isb`, `sev`, `wfe`, `wfi`, `yield` |

> Le Cortex-M0 **n'a pas** d'instructions de division ni de virgule flottante. Ces operations sont implementees par des routines logicielles.

---

## 2.4 Structure d'une Instruction

### 2.4.1 Instruction en langage assembleur (UAL)

Forme generale :
```
<operation>{S} <registre destination>, <operandes source>
```

**Exemple :**
```asm
ADDS R3, R2, R1    ; R3 = R2 + R1, met a jour les drapeaux (S)
```

- `ADDS` : operation (addition avec mise a jour des drapeaux)
- `R3` : registre destination
- `R2, R1` : operandes source

> Sur Cortex-M0, le suffixe **S** est **obligatoire** pour presque toutes les instructions de traitement de donnees (les drapeaux sont toujours mis a jour).

### 2.4.2 Code instruction (encodage binaire)

La plupart des instructions Cortex-M0 sont encodees sur **16 bits** :

```
+--------+-----+-----+-----+
| Opcode |  Rd | op1 | op2 |
+--------+-----+-----+-----+
   Bits de poids fort     Bits de poids faible
```

**Exemple : ADDS R3, R2, R1**
```
Binaire :  0001100 001 010 011
           ├─────┤ ├─┤ ├─┤ ├─┤
            ADD    R1  R2  R3     Code hex : 0x1853
            opcode  Rm  Rn  Rd
```

> **Jeux d'instructions ARM vs Thumb** :
> - Instructions **ARM** : toujours 32 bits, alignees sur 4 octets
> - Instructions **Thumb** : 16 ou 32 bits, alignees sur 2 octets
> - **Cortex-M0** utilise Thumb (sous-ensemble compact, principalement 16 bits)
> - Le bit LSB des adresses de fonctions doit etre 1 (indique le mode Thumb)

---

## 2.5 Instructions de Traitement de Donnees

### 2.5.1 Structure

```
<operation>{S} <destination>, <operandes source>
```

**Trois formes d'operandes :**

| Forme | Syntaxe | Exemple |
|-------|---------|---------|
| Registres | `<op> Rd, Rn, Rm` | `ADDS R3, R2, R1` → R3 = R2 + R1 |
| Immediat | `<op> Rd, Rn, #imm3` | `ADDS R3, R2, #1` → R3 = R2 + 1 |
| Destination = source | `<op> Rdn, <op2>` | `ADDS R7, #100` → R7 = R7 + 100 |

### 2.5.2 Instructions arithmetiques

| Instruction | Exemple | Description |
|------------|---------|-------------|
| `MOVS Rd, #imm8` | `MOVS R5, #52` | R5 ← 52 (immediat 0-255) |
| `MOV Rd, Rn` | `MOV R5, R7` | R5 ← R7 (sans modifier APSR) |
| `MOVS Rd, Rn` | `MOVS R5, R7` | R5 ← R7 (met a jour N et Z) |
| `ADDS Rd, Rn, Rm` | `ADDS R1, R2, R3` | R1 ← R2 + R3 |
| `ADDS Rd, Rn, #imm3` | `ADDS R2, R0, #5` | R2 ← R0 + 5 (imm 0-7) |
| `ADDS Rdn, #imm8` | `ADDS R1, #123` | R1 ← R1 + 123 (imm 0-255) |
| `SUBS Rd, Rn, Rm` | `SUBS R2, R3, R4` | R2 ← R3 - R4 |
| `SUBS Rd, Rn, #imm3` | `SUBS R2, R0, #5` | R2 ← R0 - 5 |
| `MULS Rdm, Rn, Rdm` | `MULS R1, R2, R1` | R1 ← R2 * R1 (Rd = Rm) |
| `CMP Rn, Rm` | `CMP R1, R2` | R1 - R2 (drapeaux seulement) |
| `CMP Rn, #imm8` | `CMP R0, #5` | R0 - 5 (drapeaux seulement) |
| `CMN Rn, Rm` | `CMN R1, R2` | R1 + R2 (drapeaux seulement) |
| `ADC Rdn, Rm` | `ADCS R2, R3` | R2 ← R2 + R3 + C |
| `SBC Rdn, Rm` | `SBCS R2, R3` | R2 ← R2 - R3 - ~C |
| `RSBS Rd, Rn, #0` | `RSBS R1, R2, #0` | R1 ← 0 - R2 (negation) |

### 2.5.3 Instructions logiques

| Instruction | Exemple | Description |
|------------|---------|-------------|
| `ANDS Rdn, Rm` | `ANDS R1, R2` | R1 ← R1 AND R2 (bit a bit) |
| `ORRS Rdn, Rm` | `ORRS R0, R7` | R0 ← R0 OR R7 |
| `EORS Rdn, Rm` | `EORS R3, R4` | R3 ← R3 XOR R4 |
| `MVNS Rd, Rm` | `MVNS R1, R2` | R1 ← NOT R2 |
| `BICS Rdn, Rm` | `BICS R1, R2` | R1 ← R1 AND (NOT R2) |
| `TST Rn, Rm` | `TST R1, R2` | R1 AND R2 (drapeaux seulement) |
| `LSLS Rd, Rm, #imm5` | `LSLS R1, R1, #3` | R1 ← R1 << 3 |
| `LSRS Rd, Rm, #imm5` | `LSRS R2, R2, #4` | R2 ← R2 >> 4 (logique) |
| `ASRS Rd, Rm, #imm5` | `ASRS R1, R1, #2` | R1 ← R1 >> 2 (arithmetique) |
| `RORS Rdn, Rm` | `RORS R1, R2` | R1 ← rotation droite de R2 bits |

---

## 2.6 Instructions d'Acces Memoire

### 2.6.1 Load-Store simple

```
LDR Rt, [Rn]     ; Rt ← Memoire[Rn]        (charge mot 32 bits)
STR Rt, [Rn]     ; Memoire[Rn] ← Rt        (stocke mot 32 bits)
```

**Exemple :**
```asm
LDR R3, [R4]      ; Charge le mot a l'adresse dans R4 vers R3
STR R3, [R4]      ; Stocke R3 a l'adresse dans R4
```

```
Encodage :
LDR R3, [R4]    0110100000 100 011    0x6823
STR R3, [R4]    0110000000 100 011    0x6023
                 opcode     Rn  Rt
```

### 2.6.2 Load-Store avec base et offset

| Forme | Syntaxe | Description |
|-------|---------|-------------|
| Offset immediat | `LDR Rt, [Rn, #imm5]` | Adresse = Rn + imm (mots) |
| Offset registre | `LDR Rt, [Rn, Rm]` | Adresse = Rn + Rm |

```asm
LDR R0, [R5, #12]    ; R0 ← Memoire[R5 + 12]
STR R0, [R5, R2]     ; Memoire[R5 + R2] ← R0
```

### 2.6.3 Tailles de transfert

| Instruction | Taille | Extension |
|------------|--------|-----------|
| `LDR` / `STR` | 32 bits (mot) | - |
| `LDRH` / `STRH` | 16 bits (demi-mot) | Zero |
| `LDRB` / `STRB` | 8 bits (octet) | Zero |
| `LDRSH` | 16 bits | **Signe** (extension de signe) |
| `LDRSB` | 8 bits | **Signe** (extension de signe) |

### 2.6.4 Load-Store multiple

```asm
STM R5!, {R0-R3, R7}    ; Stocke R0,R1,R2,R3,R7 a partir de [R5]
                          ; R5 incremente de 5*4 = 20
LDM R5!, {R0-R3, R7}    ; Charge 5 registres depuis [R5]
```

### 2.6.5 Instructions de pile

```asm
PUSH {R4-R7, LR}        ; Sauvegarde R4,R5,R6,R7,LR sur la pile
POP  {R4-R7, PC}        ; Restaure et branche (LR → PC)
```

```
Encodage PUSH :
PUSH {R4-R7}     10110100 01110000    0xB470
                  opcode   reg_list (1 bit par registre)

POP {R4-R7}      10111100 01110000    0xBC70
```

> PUSH peut aussi sauvegarder **LR** (bit supplementaire), et POP peut restaurer vers **PC** (retour de fonction).

---

## 2.7 Instructions de Controle de Programme

### 2.7.1 Branchement inconditionnel

```asm
B <label>            ; PC ← adresse du label
```

```
Encodage :  11100 | imm11
Le saut est de ±2048 octets (imm11 en demi-mots)
```

### 2.7.2 Branchement indirect

```asm
BX Rm                ; PC ← Rm (avec changement de mode Arm/Thumb)
MOV PC, Rm           ; PC ← Rm (sans changement de mode)
```

### 2.7.3 Branchement conditionnel

```asm
B<cc> <label>        ; Si condition vraie, PC ← adresse du label
```

**Exemple :**
```asm
CMP  R0, #5          ; Compare R0 avec 5
BEQ  L2              ; Si R0 == 5 (Z=1), branche vers L2
BNE  L3              ; Si R0 != 5 (Z=0), branche vers L3
```

```
Encodage :  1101 | cond(4 bits) | imm8
Portee : ±256 octets (plus courte que B inconditionnel)
```

| Code | Condition | Signification | Drapeaux |
|------|-----------|---------------|----------|
| `EQ` | Equal | Egal | Z = 1 |
| `NE` | Not Equal | Different | Z = 0 |
| `CS`/`HS` | Carry Set | Non signe ≥ | C = 1 |
| `CC`/`LO` | Carry Clear | Non signe < | C = 0 |
| `MI` | Minus | Negatif | N = 1 |
| `PL` | Plus | Positif ou zero | N = 0 |
| `VS` | Overflow Set | Debordement | V = 1 |
| `VC` | Overflow Clear | Pas de debordement | V = 0 |
| `HI` | Higher | Non signe > | C=1 et Z=0 |
| `LS` | Lower or Same | Non signe ≤ | C=0 ou Z=1 |
| `GE` | Greater/Equal | Signe ≥ | N = V |
| `LT` | Less Than | Signe < | N ≠ V |
| `GT` | Greater Than | Signe > | Z=0 et N=V |
| `LE` | Less/Equal | Signe ≤ | Z=1 ou N≠V |

### 2.7.4 Appel de fonction et retour

```asm
BL  <label>          ; LR ← PC+4 (adresse retour), PC ← label
                      ; Instruction 32 bits, portee ±16 Mo

BLX <Rm>             ; LR ← PC+2, PC ← Rm (appel indirect)

BX  LR               ; Retour : PC ← LR
```

```
Mecanisme d'appel-retour :

   Appelant             Appele (fonction)
   --------             -----------------
   BL func1     ───→    func1:
   (suite...)   ←───       ...
                           BX LR (retour)
```

---

## 2.8 Resume

### ISA du Cortex-M0
- Definit le **jeu de registres** et le **jeu d'instructions**

### Registres
- **R0-R7** : registres bas (13 registres a usage general R0-R12)
- **R13/SP** : pointeur de pile
- **R14/LR** : registre de lien (adresse de retour)
- **R15/PC** : compteur de programme
- **APSR** : drapeaux N, Z, C, V

### Structure d'une instruction
- Syntaxe : `operation Rd, op1, op2`
- Encodage : principalement **16 bits** (certaines 32 bits)
- Defini en **UAL** (Unified Assembly Language)

### Jeu d'instructions (56 instructions)
- **Traitement de donnees** : arithmetique (ADD, SUB, MUL, CMP) + logique (AND, OR, XOR, decalages)
- **Acces memoire** : load-store (LDR/STR avec variantes) + pile (PUSH/POP)
- **Controle programme** : branchement (B, BX, B<cc>) + sous-programme (BL, BLX, BX LR)
- **Controle systeme** : instructions OS et synchronisation

---

[<< Chapitre 1](./01-Systeme-Microprocesseur.md) | [Chapitre 3 : Execution des Programmes >>](./03-Execution-Programme.md)
