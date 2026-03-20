# Chapitre 13 : Fonctions en C

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 12](./12-Programmation-C.md) | [Chapitre 14 >>](./14-Tableaux-Structures.md)

---

## Objectifs

> Maitriser les fonctions C sur ARM : convention d'appel AAPCS, interaction C/assembleur, recursivite.

---

## 13.1 Definition et Appel

```c
// Declaration (prototype)
int multiply_add(int a, int b, int c);

// Definition
int multiply_add(int a, int b, int c) {
    return a * b + c;
}

// Appel
int result = multiply_add(7, 10, 11);  // result = 81
```

---

## 13.2 Convention AAPCS sur ARM

### Code genere par le compilateur

```c
int multiply_add(int a, int b, int c) {
    return a * b + c;
}
```

```asm
// a=R0, b=R1, c=R2
multiply_add:
    muls r0, r1, r0       // r0 = a * b
    adds r0, r0, r2       // r0 = a*b + c
    bx lr                  // retour (resultat dans r0)
```

### Tableau recapitulatif

| Registre | AAPCS | Sauvegarde |
|----------|-------|-----------|
| R0-R3 | Parametres 1-4 / Retour (R0) | Appelant |
| R4-R11 | Variables locales | **Appele** |
| R12 | Scratch (IP) | Appelant |
| R13/SP | Pointeur de pile | Doit etre restaure |
| R14/LR | Adresse de retour | Appele (si modifie) |

---

## 13.3 Fonctions et Pile

Le compilateur genere automatiquement le prologue/epilogue :

```c
int complex_func(int a, int b) {
    int x = a + 1;
    int y = b * 2;
    return helper(x, y);
}
```

```asm
complex_func:
    push {r4, r5, lr}     // PROLOGUE : sauvegarder
    adds r4, r0, #1       // x = a + 1 (dans r4, preservé)
    lsls r5, r1, #1       // y = b * 2 (dans r5, preservé)
    mov r0, r4             // parametre 1 = x
    mov r1, r5             // parametre 2 = y
    bl helper              // appel
    pop {r4, r5, pc}       // EPILOGUE : restaurer et retourner
```

---

## 13.4 Appel Assembleur depuis C et Vice Versa

### Appeler une fonction assembleur depuis C

```c
// En C : declarer la fonction externe
extern int asm_add(int a, int b);

int main(void) {
    int result = asm_add(3, 4);  // result = 7
}
```

```asm
// En assembleur : definir la fonction
    .global asm_add
    .thumb_func
asm_add:
    adds r0, r0, r1       // R0 = a + b
    bx lr                  // retour
```

### Appeler une fonction C depuis l'assembleur

```asm
    .extern c_function     // fonction definie en C

    // Preparer les parametres
    movs r0, #10           // 1er parametre
    movs r1, #20           // 2eme parametre
    bl c_function           // appel de la fonction C
    // resultat dans r0
```

---

## 13.5 Recursivite

```c
// Factorielle recursive
uint32_t factorial(uint32_t n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

```asm
factorial:
    push {r4, lr}          // sauvegarder (appel imbrique)
    mov r4, r0              // sauvegarder n
    cmp r0, #1
    ble base_case

    subs r0, r4, #1        // r0 = n - 1
    bl factorial             // appel recursif
    muls r0, r4, r0        // r0 = n * factorial(n-1)
    pop {r4, pc}

base_case:
    movs r0, #1            // retourner 1
    pop {r4, pc}
```

> **Attention** : La recursion consomme de la pile a chaque appel. Sur un microcontroleur avec peu de RAM, risque de **stack overflow** !

---

## Resume

- AAPCS : R0-R3 parametres, R0 retour, R4-R11 preserves
- Prologue : `PUSH {Rx, LR}` / Epilogue : `POP {Rx, PC}`
- C ↔ Assembleur : `.global` + `.thumb_func` + convention AAPCS
- Recursivite : possible mais attention a la taille de la pile

---

[<< Chapitre 12](./12-Programmation-C.md) | [Chapitre 14 : Tableaux et Structures >>](./14-Tableaux-Structures.md)
