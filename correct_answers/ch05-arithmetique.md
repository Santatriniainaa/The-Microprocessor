# Corriges - Chapitre 5 : Operations Arithmetiques

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Exercice 5.1

### Addition (Serie de Fibonacci)

Le programme genere la suite de Fibonacci : 1, 1, 2, 3, 5, 8, 13, 21...

```asm
    .syntax unified
    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    movs r1, #1         // r1 = 1
    movs r2, #1         // r2 = 1
    adds r3, r2, r1     // r3 = 2  (1+1)
    adds r1, r3, r2     // r1 = 3  (2+1)
    adds r2, r1, r3     // r2 = 5  (3+2)
    adds r3, r2, r1     // r3 = 8  (5+3)
    adds r1, r3, r2     // r1 = 13 (8+5)
    adds r2, r1, r3     // r2 = 21 (13+8)
L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Reponse** : C'est la **suite de Fibonacci** (chaque nombre = somme des deux precedents).

| Etape | R1 | R2 | R3 |
|-------|----|----|-----|
| Init | 1 | 1 | - |
| +1 | 1 | 1 | **2** |
| +2 | **3** | 1 | 2 |
| +3 | 3 | **5** | 2 |
| +4 | 3 | 5 | **8** |
| +5 | **13** | 5 | 8 |
| +6 | 13 | **21** | 8 |

---

## Exercice 5.2

### Multiplication et debordement

```
movs r0, #0x80    // r0 = 128
movs r1, #0x80    // r1 = 128
muls r0, r1, r0   // r0 = 128 * 128 = 16384 = 0x4000        ✓
muls r0, r1, r0   // r0 = 16384 * 128 = 2097152 = 0x200000  ✓
muls r0, r1, r0   // r0 = 2097152 * 128 = 0x10000000         ✓
muls r0, r1, r0   // r0 = 0x10000000 * 128 = 0x800000000
                   //    → tronque a 32 bits = 0x00000000     ← OVERFLOW !
muls r0, r1, r0   // r0 = 0 * 128 = 0
muls r0, r1, r0   // r0 = 0 * 128 = 0
```

**Reponse** : A la 4eme multiplication, le resultat (0x800000000) depasse 32 bits. Seuls les 32 bits bas sont gardes → **0x00000000**. Apres cela, tout reste 0.

> **Note** : MULS ne met **pas** a jour C ou V. On ne peut pas detecter le debordement de multiplication via les drapeaux.

---

## Exercice 5.3

### Soustraction et nombres signes

```
movs r1, #10      // r1 = 10 = 0x0000000A
negs r1, r1       // r1 = -10 = 0xFFFFFFF6

movs r1, #0x10    // r1 = 16 = 0x00000010
subs r1, #0x20    // r1 = 16 - 32 = -16 = 0xFFFFFFF0

movs r1, #120     // r1 = 120 = 0x00000078
subs r1, #240     // r1 = 120 - 240 = -120 = 0xFFFFFF88
```

**Resultats** :
- `NEGS R1, R1` avec R1=10 : R1 = 0xFFFFFFF6 (-10 en complement a deux)
- 16 - 32 = -16 : R1 = 0xFFFFFFF0
- 120 - 240 = -120 : R1 = 0xFFFFFF88 (120 en hex = 0x78, donc -120 = 0xFFFFFF88)

---

## Exercice 5.4

### Drapeaux N et Z

| Instruction | Resultat | N | Z |
|-------------|----------|---|---|
| `movs r0, #0` | R0 = 0 | 0 | **1** |
| `adds r1, r0, #1` | R1 = 1 | 0 | 0 |
| `negs r1, r1` | R1 = -1 = 0xFFFFFFFF | **1** | 0 |
| `movs r2, #5` | R2 = 5 | 0 | 0 |
| `movs r3, #15` | R3 = 15 | 0 | 0 |
| `subs r3, r2` | R3 = 15-5 = 10 | 0 | 0 |
| `subs r3, #15` | R3 = 10-15 = -5 = 0xFFFFFFFB | **1** | 0 |
| `adds r3, r2` | R3 = -5+5 = 0 | 0 | **1** |

---

## Exercice 5.5

### Carry et Overflow

**Partie 1 : Carry (debordement non signe)**
```
R1 = 0xFFFFFFFF   // = 2^32 - 1 (max unsigned)
ADDS R1, #1       // = 0x00000000, C=1 (carry!), V=0
                   // En signe: -1 + 1 = 0, pas d'overflow → V=0
```

**Partie 2 : Overflow (debordement signe)**
```
R1 = 0x7FFFFFFF   // = 2^31 - 1 (max signed positive)
ADDS R1, #1       // = 0x80000000, C=0, V=1 (overflow!)
                   // En signe: +2147483647 + 1 = -2147483648 !
```

---

## Exercice 5.6

### Comparaison non signee

Avec R1=5, R2=7 :
```
CMP R1, R2        // 5 - 7 : resultat negatif
                   // C=0 (emprunt), Z=0
BEQ L1             // Z=0 → pas de branchement
BLO L2             // C=0 → branchement pris ! (5 < 7)
BHI L3             // non atteint
```
**Resultat** : R0 = 1 (indique R1 < R2)

---

## Exercice 5.7

### Comparaison signee

Avec R1=10, R2=-5 :
```
CMP R1, R2        // 10 - (-5) = 15 : positif
                   // N=0, V=0 → N=V → GE et GT
BEQ L1             // Z=0 → non
BLT L2             // N≠V ? Non (N=0, V=0, N=V) → non
BGT L3             // Z=0 et N=V → branchement pris ! (10 > -5)
```
**Resultat** : R0 = 2 (indique R1 > R2)

---

## Simulation : Somme d'un Tableau

> **Enonce** : Calculer la somme des elements d'un tableau de N mots en memoire.

```asm
    .syntax unified

    .data
    .global arr
    .balign 4
arr: .word 10, 20, 30, 40, 50, 60, 70, 80, 90, 100

    .equ ARR_SIZE, 10          // nombre d'elements

    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    ldr  r0, =arr              // r0 = adresse du tableau
    movs r1, #0                // r1 = index (offset en octets)
    movs r2, #0                // r2 = somme = 0
    ldr  r3, =(ARR_SIZE * 4)   // r3 = taille totale en octets (40)

sum_loop:
    cmp  r1, r3                // comparer index avec taille
    bge  sum_done              // si index >= taille, terminer
    ldr  r4, [r0, r1]         // r4 = arr[i]
    adds r2, r2, r4           // somme += arr[i]
    adds r1, r1, #4           // index += 4 (prochain mot)
    b    sum_loop

sum_done:
    // r2 = somme = 10+20+30+40+50+60+70+80+90+100 = 550 = 0x226

L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Resultat attendu** : R2 = **550** (0x226)

**Verification** : 10+20+30+40+50+60+70+80+90+100 = 10 × (10+100)/2 = 550

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
