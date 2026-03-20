# Corriges - Chapitre 9 : Stack et Fonctions

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index](./00-INDEX-CORRIGES.md)

---

## Simulation : Fonction Factorielle

```asm
    .syntax unified
    .text
    .balign 2

// uint32_t factorial(uint32_t n)
// Parametres : R0 = n
// Retour : R0 = n!
    .global factorial
    .thumb_func
factorial:
    push {r4, lr}             // sauvegarder r4 et LR
    mov  r4, r0               // r4 = n (sauvegarde)
    cmp  r0, #1
    ble  fact_base             // si n <= 1, retourner 1

    subs r0, r4, #1           // r0 = n - 1
    bl   factorial             // r0 = factorial(n-1)
    muls r0, r4, r0           // r0 = n * factorial(n-1)
    pop  {r4, pc}             // retour

fact_base:
    movs r0, #1               // retourner 1
    pop  {r4, pc}

// Programme principal
    .global asm_main
    .thumb_func
asm_main:
    movs r0, #5               // calculer 5!
    bl   factorial             // r0 = 120
    mov  r5, r0               // r5 = resultat = 120

L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Resultat** : 5! = 5 × 4 × 3 × 2 × 1 = **120** (0x78)

### Trace de la pile pour factorial(5)

```
Appel factorial(5) : PUSH {R4=5, LR=ret_main}
  Appel factorial(4) : PUSH {R4=4, LR=ret_f5}
    Appel factorial(3) : PUSH {R4=3, LR=ret_f4}
      Appel factorial(2) : PUSH {R4=2, LR=ret_f3}
        Appel factorial(1) : PUSH {R4=1, LR=ret_f2}
          → retourne 1
        POP → R0 = 2*1 = 2
      POP → R0 = 3*2 = 6
    POP → R0 = 4*6 = 24
  POP → R0 = 5*24 = 120
```

---

## Simulation : Fonction GCD (Plus Grand Commun Diviseur)

### Algorithme d'Euclide

```
gcd(a, b) :
    si b == 0, retourner a
    sinon retourner gcd(b, a % b)
```

Comme le Cortex-M0 n'a pas d'instruction MOD, on utilise la soustraction repetee :

```asm
    .syntax unified
    .text
    .balign 2

// uint32_t gcd(uint32_t a, uint32_t b)
// R0 = a, R1 = b → R0 = gcd(a,b)
    .global gcd
    .thumb_func
gcd:
    cmp  r1, #0
    beq  gcd_done              // si b == 0, retourner a

gcd_loop:
    cmp  r0, r1
    beq  gcd_done              // si a == b, retourner a
    bgt  a_greater
    subs r1, r1, r0            // b = b - a
    b    gcd_loop
a_greater:
    subs r0, r0, r1            // a = a - b
    b    gcd_loop

gcd_done:
    bx   lr                    // retourner a (dans R0)

// Programme principal
    .global asm_main
    .thumb_func
asm_main:
    movs r0, #48               // a = 48
    movs r1, #36               // b = 36
    bl   gcd                   // r0 = gcd(48, 36) = 12
    mov  r5, r0

L0: b L0
    .size asm_main,.-asm_main
    .end
```

**Resultat** : gcd(48, 36) = **12**

### Trace : gcd(48, 36)
```
a=48, b=36 → a>b → a=48-36=12
a=12, b=36 → a<b → b=36-12=24
a=12, b=24 → a<b → b=24-12=12
a=12, b=12 → a==b → retourner 12
```

---

[Retour a l'index](./00-INDEX-CORRIGES.md)
