# Corriges - Chapitre 7 : Acces Memoire

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Exercice 7.5 - Offset immediat (reponses aux ?)

La structure en memoire :
```
Offset 0  : .word  0x12345678    (mot, 4 octets)
Offset 4  : .hword 0x1111        (demi-mot, 2 octets)
Offset 6  : .byte  0x22          (octet, 1 octet)
Offset 7  : .byte  0             (padding)
Offset 8  : .hword 0xABCD        (demi-mot, 2 octets)
Offset 10 : .hword 0             (padding)
Offset 12 : .word  0x11223344    (mot, 4 octets)
```

**Reponses :**
- 4eme membre (hword 0xABCD) : `ldrh r4, [r0, #8]` → offset = **8**
- 5eme membre (word 0x11223344) : `str r3, [r0, #12]` → offset = **12**

## Exercice 7.6 - Register offset

Le programme initialise un tableau de 256 mots a 0xFFFFFFFF :

```
Iteration | R0 (base) | R1 (offset) | Adresse effective | Action
0         | arr       | 0            | arr + 0           | Ecrire 0xFFFFFFFF
1         | arr       | 4            | arr + 4           | Ecrire 0xFFFFFFFF
2         | arr       | 8            | arr + 8           | Ecrire 0xFFFFFFFF
...
255       | arr       | 1020         | arr + 1020        | Ecrire 0xFFFFFFFF
```

La boucle s'arrete quand R1 (offset) atteint R3 (256×4 = 1024).

---

## Simulation : Copier / Inverser un Tableau

### Programme 1 : Copier un tableau

```asm
    .syntax unified

    .data
    .balign 4
src:  .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    .balign 4
dst:  .space 40                // 10 mots = 40 octets

    .equ N, 10

    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    ldr  r0, =src              // source
    ldr  r1, =dst              // destination
    movs r2, #0                // index (en octets)
    ldr  r3, =(N * 4)          // taille totale

copy_loop:
    cmp  r2, r3
    bge  copy_done
    ldr  r4, [r0, r2]         // lire depuis source
    str  r4, [r1, r2]         // ecrire dans destination
    adds r2, #4
    b    copy_loop

copy_done:
    // dst[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
```

### Programme 2 : Inverser un tableau (en place)

```asm
    .syntax unified

    .data
    .balign 4
arr: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

    .equ N, 10

    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    ldr  r0, =arr              // adresse du tableau
    movs r1, #0                // index gauche (en octets)
    ldr  r2, =((N - 1) * 4)   // index droit (en octets) = 36

reverse_loop:
    cmp  r1, r2                // si gauche >= droit, fini
    bge  reverse_done

    // Echanger arr[gauche] et arr[droit]
    ldr  r3, [r0, r1]         // r3 = arr[gauche]
    ldr  r4, [r0, r2]         // r4 = arr[droit]
    str  r4, [r0, r1]         // arr[gauche] = r4
    str  r3, [r0, r2]         // arr[droit] = r3

    adds r1, #4               // gauche++
    subs r2, #4               // droit--
    b    reverse_loop

reverse_done:
    // arr[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}

L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Resultat** : arr = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
