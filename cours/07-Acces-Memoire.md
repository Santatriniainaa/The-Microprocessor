# Chapitre 7 : Acces Memoire

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 6](./06-Operations-Niveau-Bit.md) | [Chapitre 8 >>](./08-Sous-Programmes.md)

---

## Objectifs

> Comprendre les types de donnees, l'alignement, l'endianness, et les differents modes d'adressage du Cortex-M0.

---

## 7.1 Types de Donnees et Tailles

| Type ARM | Taille | Type C | Instructions |
|----------|--------|--------|-------------|
| Octet (byte) | 8 bits | `char`, `uint8_t` | `LDRB`/`STRB` |
| Demi-mot (halfword) | 16 bits | `short`, `uint16_t` | `LDRH`/`STRH` |
| Mot (word) | 32 bits | `int`, `uint32_t` | `LDR`/`STR` |

### Instructions load avec extension

| Instruction | Taille lue | Extension | Resultat 32 bits |
|------------|-----------|-----------|------------------|
| `LDRB` | 8 bits | Zero | `0x000000XX` |
| `LDRH` | 16 bits | Zero | `0x0000XXXX` |
| `LDRSB` | 8 bits | **Signe** | `0xFFFFFFXX` si negatif |
| `LDRSH` | 16 bits | **Signe** | `0xFFFFXXXX` si negatif |
| `LDR` | 32 bits | Aucune | `0xXXXXXXXX` |

---

## 7.2 Alignement

| Acces | Alignement requis | Adresses valides |
|-------|------------------|------------------|
| Octet | Aucun | Toute adresse |
| Demi-mot | 2 octets | Adresses paires (0, 2, 4...) |
| Mot | 4 octets | Multiples de 4 (0, 4, 8...) |

```
Adresse   Alignement
0x100     ✓ Mot (÷4), ✓ Demi-mot (÷2), ✓ Octet
0x101     ✗ Mot, ✗ Demi-mot, ✓ Octet
0x102     ✗ Mot, ✓ Demi-mot, ✓ Octet
0x104     ✓ Mot, ✓ Demi-mot, ✓ Octet
```

> **Cortex-M0** : Un acces non aligne provoque un **HardFault** !
> **Cortex-M3/M4/M7** : Supportent les acces non alignes (mais plus lents).

---

## 7.3 Endianness (Boutisme)

### Little-Endian (defaut sur ARM)

L'octet de **poids faible** est stocke a l'adresse la **plus basse**.

```
Mot 0x12345678 stocke a l'adresse 0x100 :

Adresse :  0x100  0x101  0x102  0x103
Contenu :  0x78   0x56   0x34   0x12
           LSB                   MSB
```

### Big-Endian

L'octet de **poids fort** est stocke a l'adresse la plus basse.

```
Adresse :  0x100  0x101  0x102  0x103
Contenu :  0x12   0x34   0x56   0x78
           MSB                   LSB
```

> **Cortex-M0** utilise **little-endian**. Les instructions `REV`/`REV16` permettent la conversion.

---

## 7.4 Modes d'Adressage

### Tableau recapitulatif

| Mode | Syntaxe | Adresse calculee | Exemple |
|------|---------|-----------------|---------|
| Registre indirect | `[Rn]` | Rn | `LDR R0, [R1]` |
| Offset immediat | `[Rn, #imm]` | Rn + imm | `LDR R0, [R1, #8]` |
| Offset registre | `[Rn, Rm]` | Rn + Rm | `LDR R0, [R1, R2]` |
| PC-relatif | `[PC, #imm]` | PC + imm | `LDR R0, =label` |
| SP-relatif | `[SP, #imm]` | SP + imm | `LDR R0, [SP, #4]` |

### Plages d'offset

| Instruction | Offset immediat | Pas |
|------------|----------------|-----|
| `LDR/STR` (mot) | 0-124 | 4 octets |
| `LDRH/STRH` (demi-mot) | 0-62 | 2 octets |
| `LDRB/STRB` (octet) | 0-31 | 1 octet |
| `LDR` (SP-relatif) | 0-1020 | 4 octets |
| `LDR` (PC-relatif) | 0-1020 | 4 octets |

### Exemples detailles

```asm
// Registre indirect
ldr r0, [r4]              // r0 ← Mem[r4]

// Offset immediat (acces element de tableau)
ldr r0, [r4, #8]          // r0 ← Mem[r4 + 8] (3eme mot)

// Offset registre (index variable)
ldr r0, [r4, r2]          // r0 ← Mem[r4 + r2]

// SP-relatif (variables locales)
ldr r0, [sp, #4]          // r0 ← Mem[SP + 4]

// PC-relatif (constantes/adresses)
ldr r0, =ma_variable      // r0 ← adresse de ma_variable
```

---

## 7.5 Acces aux Litteraux (Pool de Constantes)

La pseudo-instruction `LDR Rd, =constante` charge une valeur 32 bits :

```asm
ldr r0, =0x12345678      // Charge 0x12345678 dans r0
```

L'assembleur traduit cela en :
1. `LDR R0, [PC, #offset]` — charge depuis un litteral proche
2. `.word 0x12345678` — la constante placee apres la fonction

```
Code :     LDR R0, [PC, #offset]   ← instruction reelle
           ...
Litteral : .word 0x12345678         ← pool de constantes
```

---

## 7.6 Load-Store Multiple

```asm
// Stocker plusieurs registres en memoire
stm r5!, {r0-r3, r7}     // Stocke R0,R1,R2,R3,R7 a partir de [R5]
                           // R5 incremente de 5×4 = 20 octets

// Charger plusieurs registres depuis memoire
ldm r5!, {r0-r3, r7}     // Charge 5 mots depuis [R5]
                           // R5 incremente de 20 octets
```

**Ordre en memoire** : toujours du registre le plus petit au plus grand.

```
Adresse        Registre
R5 + 0    ←→   R0
R5 + 4    ←→   R1
R5 + 8    ←→   R2
R5 + 12   ←→   R3
R5 + 16   ←→   R7
```

---

## Resume

- **Types** : octet (8), demi-mot (16), mot (32) avec extensions signe/zero
- **Alignement** : mot sur 4, demi-mot sur 2 — HardFault si non aligne (M0)
- **Endianness** : Cortex-M0 = little-endian (LSB a l'adresse basse)
- **Adressage** : indirect, offset immediat/registre, PC-relatif, SP-relatif
- **Litteraux** : `LDR Rd, =valeur` → pool de constantes
- **Multiple** : LDM/STM pour transferts en bloc

---

[<< Chapitre 6](./06-Operations-Niveau-Bit.md) | [Chapitre 8 : Sous-Programmes >>](./08-Sous-Programmes.md)
