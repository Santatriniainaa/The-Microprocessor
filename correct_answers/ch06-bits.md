# Corriges - Chapitre 6 : Operations au Niveau Bit

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Exercice 6.1

### Operations Booleennes

```
R0 = 0xFF555500, R1 = 0xFFAAAA00

AND : R2 = 0xFF555500 AND 0xFFAAAA00 = 0xFF000000
OR  : R2 = 0xFF555500 OR  0xFFAAAA00 = 0xFFFFFF00
XOR : R2 = 0xFF555500 XOR 0xFFAAAA00 = 0x00FFFF00
NOT(R0) : R2 = NOT 0xFF555500 = 0x00AAAAFF
NOT(R1) : R2 = NOT 0xFFAAAA00 = 0x005555FF
```

## Exercice 6.2

### Masquage de bits (Set, Clear, Test)

```
Masque = 0x000F0000 (bits 16-19)
Original = 0x5A5A5A5A

SET bits 16-19 :  0x5A5A5A5A OR 0x000F0000  = 0x5A5F5A5A
CLEAR bits 16-19: 0x5A5A5A5A AND ~0x000F0000 = 0x5A505A5A
                  (equivalent: BICS avec masque 0x000F0000)

TEST bit 16 :    0xFFFF0000 AND (1<<16) = 0x00010000 ≠ 0 → bit 16 = 1 (Z=0)
TEST bit 3 :     0xFFFF0000 AND (1<<3)  = 0x00000000 = 0 → bit 3 = 0 (Z=1)
```

## Exercice 6.3

### Decalages logiques

```
0x07 << 14 :
  0x07 = 0b00000000000000000000000000000111
  << 14 = 0b00000000000000011100000000000000 = 0x0001C000

0xA8000000 >> 27 :
  0xA8000000 = 0b10101000000000000000000000000000
  >> 27      = 0b00000000000000000000000000010101 = 0x00000015
```

## Exercice 6.4

### Decalages arithmetiques vs logiques

**Nombre positif (0x10 = 16) :**
```
ASRS R1, #4 → 16 / 16 = 1     (identique a LSRS)
LSRS R2, #4 → 16 / 16 = 1     (identique)
```

**Nombre negatif (0xFFFFFFF0 = -16) :**
```
ASRS R1, #4 → -16 / 16 = -1 = 0xFFFFFFFF  (signe preserve)
LSRS R2, #4 → 0x0FFFFFFF = 268435455      (zeros inseres, signe perdu !)
```

**Boucle avec 0x80000000 :**
- ASRS : bit de signe se propage → apres 32 shifts, R1 = 0xFFFFFFFF
- LSRS : zeros inseres → apres 32 shifts, R2 = 0x00000000

## Exercice 6.5

### Extension de longueur de mot

```
R0 = 5 (0x00000005)

UXTB R1, R0 → zero-extend(0x05) = 0x00000005
SXTB R2, R0 → sign-extend(0x05) = 0x00000005  (bit 7=0, positif)

NEGS R0, R0 → R0 = -5 = 0xFFFFFFFB

UXTB R1, R0 → zero-extend(0xFB) = 0x000000FB = 251 (non signe)
SXTB R2, R0 → sign-extend(0xFB) = 0xFFFFFFFB = -5  (signe preserve)

UXTH R1, R0 → zero-extend(0xFFFB) = 0x0000FFFB = 65531
SXTH R2, R0 → sign-extend(0xFFFB) = 0xFFFFFFFB = -5
```

---

## Simulation : Compter les Bits a 1

> **Enonce** : Ecrire un programme qui compte le nombre de bits a 1 dans un mot de 32 bits.

```asm
    .syntax unified

    .data
    .global value
    .balign 4
value: .word 0xA5A5A5A5      // 10100101 repete 4 fois → 16 bits a 1

    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    ldr  r0, =value
    ldr  r0, [r0]             // r0 = valeur a analyser
    movs r1, #0               // r1 = compteur de bits a 1
    movs r2, #32              // r2 = nombre de bits a tester

count_loop:
    cmp  r2, #0
    beq  count_done

    movs r3, #1               // masque = bit 0
    tst  r0, r3               // tester le bit 0
    beq  bit_is_zero
    adds r1, r1, #1           // si bit=1, incrementer compteur

bit_is_zero:
    lsrs r0, r0, #1           // decaler a droite (prochain bit)
    subs r2, r2, #1           // decrementer le compteur de bits
    b    count_loop

count_done:
    // r1 = nombre de bits a 1

L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Resultat** : Pour 0xA5A5A5A5 = 1010 0101 1010 0101 1010 0101 1010 0101
- Chaque octet `0xA5` a 4 bits a 1
- 4 octets × 4 bits = **R1 = 16**

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
