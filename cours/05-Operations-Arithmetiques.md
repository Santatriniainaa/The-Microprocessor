# Chapitre 5 : Operations Arithmetiques

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 4](./04-Programmation-Assembleur.md) | [Chapitre 6 >>](./06-Operations-Niveau-Bit.md)

---

## Objectifs

> Comprendre les operations arithmetiques du Cortex-M0, la representation des nombres signes/non signes, et les drapeaux APSR.

---

## 5.1 Instructions Arithmetiques

### Table complete

| Instruction | Syntaxe | Description |
|------------|---------|-------------|
| `MOVS` | `MOVS Rd, #imm8` | Rd ← imm (0-255) |
| `MOV` | `MOV Rd, Rm` | Rd ← Rm (sans drapeaux) |
| `ADDS` | `ADDS Rd, Rn, Rm` | Rd ← Rn + Rm |
| `ADDS` | `ADDS Rd, Rn, #imm3` | Rd ← Rn + imm (0-7) |
| `ADDS` | `ADDS Rdn, #imm8` | Rdn ← Rdn + imm (0-255) |
| `ADCS` | `ADCS Rdn, Rm` | Rdn ← Rdn + Rm + C |
| `SUBS` | `SUBS Rd, Rn, Rm` | Rd ← Rn - Rm |
| `SUBS` | `SUBS Rd, Rn, #imm3` | Rd ← Rn - imm (0-7) |
| `SBCS` | `SBCS Rdn, Rm` | Rdn ← Rdn - Rm - ~C |
| `RSBS` | `RSBS Rd, Rn, #0` | Rd ← 0 - Rn (negation) |
| `MULS` | `MULS Rdm, Rn, Rdm` | Rdm ← Rn × Rdm |
| `CMP` | `CMP Rn, Rm` | Rn - Rm (drapeaux uniquement) |
| `CMP` | `CMP Rn, #imm8` | Rn - imm (drapeaux uniquement) |
| `CMN` | `CMN Rn, Rm` | Rn + Rm (drapeaux uniquement) |
| `SXTB` | `SXTB Rd, Rm` | Extension de signe octet → mot |
| `SXTH` | `SXTH Rd, Rm` | Extension de signe demi-mot → mot |
| `UXTB` | `UXTB Rd, Rm` | Extension zero octet → mot |
| `UXTH` | `UXTH Rd, Rm` | Extension zero demi-mot → mot |

> **Cortex-M0** : Pas de division (`DIV`), pas de virgule flottante (`FPU`). Utiliser des routines logicielles.

---

## 5.2 Nombres Non Signes (Unsigned)

### Representation

Pour **N bits**, plage : **0** a **2^N - 1**

| Bits | Plage | Maximum |
|------|-------|---------|
| 8 | 0 - 255 | 0xFF |
| 16 | 0 - 65 535 | 0xFFFF |
| 32 | 0 - 4 294 967 295 | 0xFFFFFFFF |

### Addition non signee

```
  0xFFFFFFF0   (4 294 967 280)
+ 0x00000020   (32)
-----------
= 0x00000010   (16)  ← Resultat tronque a 32 bits
  Carry = 1          ← Debordement non signe !
```

### Soustraction non signee

```
  0x00000010   (16)
- 0x00000020   (32)
-----------
= 0xFFFFFFF0   (4 294 967 280)  ← Resultat "wraparound"
  Carry = 0                      ← Emprunt (A < B)
```

---

## 5.3 Drapeaux APSR pour Operations Non Signees

| Drapeau | Signification pour non signe |
|---------|------------------------------|
| **Z** (Zero) | 1 si resultat = 0 |
| **C** (Carry) | Addition : 1 si retenue (debordement) |
| | Soustraction : 0 si emprunt (A < B) |
| **N** | Bit 31 du resultat (pas de sens pour non signe) |

### Exemples

```
ADDS R2, R0, R1   avec R0 = 5, R1 = 3
→ R2 = 8,  N=0, Z=0, C=0, V=0

ADDS R2, R0, R1   avec R0 = 0xFFFFFFFF, R1 = 1
→ R2 = 0,  N=0, Z=1, C=1, V=0  (debordement non signe)

SUBS R2, R0, R1   avec R0 = 3, R1 = 5
→ R2 = 0xFFFFFFFE, N=1, Z=0, C=0, V=0  (emprunt)

CMP R0, R1        avec R0 = R1
→ N=0, Z=1, C=1, V=0  (egal)
```

---

## 5.4 Nombres Signes (Signed) - Complement a Deux

### Representation

Pour **N bits**, plage : **-2^(N-1)** a **2^(N-1) - 1**

| Bits | Plage | Min | Max |
|------|-------|-----|-----|
| 8 | -128 a 127 | 0x80 | 0x7F |
| 16 | -32 768 a 32 767 | 0x8000 | 0x7FFF |
| 32 | -2 147 483 648 a 2 147 483 647 | 0x80000000 | 0x7FFFFFFF |

### Calcul du complement a deux

Pour obtenir **-X** : inverser tous les bits, puis ajouter 1.

```
 5 en 8 bits :  00000101
Inversion :     11111010
Ajouter 1 :     11111011  = -5 (0xFB)
```

### Extension de signe vs Extension de zero

```
Octet signe -5 = 0xFB = 11111011

Extension de SIGNE (SXTB) → 0xFFFFFFFB  (reste -5 en 32 bits)
Extension de ZERO  (UXTB) → 0x000000FB  (= 251 en non signe)
```

| Instruction | Operation |
|------------|-----------|
| `SXTB Rd, Rm` | Etend le signe du bit 7 aux bits 8-31 |
| `SXTH Rd, Rm` | Etend le signe du bit 15 aux bits 16-31 |
| `UXTB Rd, Rm` | Met les bits 8-31 a zero |
| `UXTH Rd, Rm` | Met les bits 16-31 a zero |

---

## 5.5 Drapeaux APSR pour Operations Signees

| Drapeau | Signification pour signe |
|---------|--------------------------|
| **N** (Negative) | 1 si resultat negatif (bit 31 = 1) |
| **V** (oVerflow) | 1 si debordement signe |
| **Z** (Zero) | 1 si resultat = 0 |

### Detection du debordement signe (V)

**Addition** : V = 1 si les deux operandes ont le **meme signe** mais le resultat a un **signe different**.

```
  0x7FFFFFFF   (+2 147 483 647)
+ 0x00000001   (+1)
= 0x80000000   (-2 147 483 648)  ← V=1 ! (positif + positif = negatif)
```

**Soustraction** : V = 1 si les operandes ont des **signes differents** et le resultat est inattendu.

### Comparaison signee : N et V ensemble

| Condition | Drapeaux | Signification |
|-----------|----------|---------------|
| A = B | Z = 1 | Egal |
| A ≠ B | Z = 0 | Different |
| A > B (signe) | Z=0 et N=V | Strictement superieur |
| A ≥ B (signe) | N = V | Superieur ou egal |
| A < B (signe) | N ≠ V | Strictement inferieur |
| A ≤ B (signe) | Z=1 ou N≠V | Inferieur ou egal |

---

## 5.6 Codes de Condition (pour branchements)

### Table complete des 16 codes

| Code | Nom | Condition | Drapeaux | Utilisation |
|------|-----|-----------|----------|-------------|
| `EQ` | Equal | Egal | Z=1 | Entiers signes/non signes |
| `NE` | Not Equal | Different | Z=0 | Entiers signes/non signes |
| `CS`/`HS` | Carry Set / Higher or Same | Non signe ≥ | C=1 | Non signe |
| `CC`/`LO` | Carry Clear / Lower | Non signe < | C=0 | Non signe |
| `MI` | Minus | Negatif | N=1 | Signe |
| `PL` | Plus | Positif ou zero | N=0 | Signe |
| `VS` | Overflow Set | Debordement | V=1 | Signe |
| `VC` | Overflow Clear | Pas debordement | V=0 | Signe |
| `HI` | Higher | Non signe > | C=1 et Z=0 | Non signe |
| `LS` | Lower or Same | Non signe ≤ | C=0 ou Z=1 | Non signe |
| `GE` | Greater or Equal | Signe ≥ | N=V | Signe |
| `LT` | Less Than | Signe < | N≠V | Signe |
| `GT` | Greater Than | Signe > | Z=0 et N=V | Signe |
| `LE` | Less or Equal | Signe ≤ | Z=1 ou N≠V | Signe |
| `AL` | Always | Toujours | - | Inconditionnel |

### Exemple d'utilisation

```asm
    cmp  r0, #10         // Comparer r0 avec 10
    beq  est_egal        // Si r0 == 10, aller a est_egal
    bgt  est_plus_grand  // Si r0 > 10 (signe), aller la
    blt  est_plus_petit  // Si r0 < 10 (signe), aller la

    cmp  r0, r1          // Comparer r0 avec r1
    bhi  r0_superieur    // Si r0 > r1 (non signe)
    bls  r0_inf_ou_egal  // Si r0 <= r1 (non signe)
```

---

## Resume

- **Non signe** : 0 a 2^N-1, utiliser C pour debordement, codes HI/HS/LO/LS
- **Signe** : complement a deux, -2^(N-1) a 2^(N-1)-1, utiliser V pour debordement, codes GT/GE/LT/LE
- **Drapeaux** : N (negatif), Z (zero), C (carry), V (overflow)
- **Extension** : SXTB/SXTH (signe), UXTB/UXTH (zero)
- **Pas de DIV/FPU** sur Cortex-M0

---

[<< Chapitre 4](./04-Programmation-Assembleur.md) | [Chapitre 6 : Operations au Niveau Bit >>](./06-Operations-Niveau-Bit.md)
