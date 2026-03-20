# Corriges - Chapitre 8 : Branch et Structures de Controle

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Simulation : Tri d'un Tableau (Bubble Sort)

> **Enonce** : Trier un tableau d'entiers en ordre croissant en assembleur ARM.

### Algorithme Bubble Sort en pseudo-code

```
pour i de 0 a N-2 :
    pour j de 0 a N-2-i :
        si arr[j] > arr[j+1] :
            echanger arr[j] et arr[j+1]
```

### Implementation ARM Cortex-M0

```asm
    .syntax unified

    .data
    .balign 4
arr: .word 64, 25, 12, 22, 11, 90, 45, 33
    .equ N, 8

    .text
    .balign 2
    .global asm_main
    .thumb_func
asm_main:
    ldr  r0, =arr              // r0 = adresse base du tableau
    movs r5, #(N - 1)          // r5 = N-1 (boucle externe)

outer_loop:
    cmp  r5, #0
    ble  sort_done

    movs r1, #0                // r1 = j (index boucle interne, en octets)
    mov  r6, r5                // r6 = limite interne
    lsls r6, r6, #2            // r6 = limite en octets

inner_loop:
    cmp  r1, r6
    bge  inner_done

    ldr  r2, [r0, r1]         // r2 = arr[j]
    adds r3, r1, #4
    ldr  r4, [r0, r3]         // r4 = arr[j+1]

    cmp  r2, r4                // comparer arr[j] et arr[j+1]
    ble  no_swap               // si arr[j] <= arr[j+1], pas d'echange

    // Echanger
    str  r4, [r0, r1]         // arr[j] = arr[j+1]
    str  r2, [r0, r3]         // arr[j+1] = arr[j]

no_swap:
    adds r1, #4               // j++
    b    inner_loop

inner_done:
    subs r5, #1               // i++
    b    outer_loop

sort_done:
    // arr[] = {11, 12, 22, 25, 33, 45, 64, 90}

L0: b L0
    .size asm_main,.-asm_main
    .end
```

### Trace d'execution

```
Initial : [64, 25, 12, 22, 11, 90, 45, 33]
Passe 1 : [25, 12, 22, 11, 64, 45, 33, 90]
Passe 2 : [12, 22, 11, 25, 45, 33, 64, 90]
Passe 3 : [12, 11, 22, 25, 33, 45, 64, 90]
...
Final   : [11, 12, 22, 25, 33, 45, 64, 90]
```

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
