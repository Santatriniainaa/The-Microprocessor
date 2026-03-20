# Chapitre 9 : Pile et Variables Locales

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 8](./08-Sous-Programmes.md) | [Chapitre 10 >>](./10-Modele-Exception.md)

---

## Objectifs

> Comprendre le fonctionnement de la pile, les cadres de pile, et l'allocation des variables locales sur la pile.

---

## 9.1 La Pile (Stack)

### Concept LIFO

La pile fonctionne en **LIFO** (Last In, First Out) : le dernier element empile est le premier depile.

```
Adresses hautes
    |           |
    |  (libre)  |
    +-----------+
    |  Donnee 3 |  ← SP (sommet de la pile)
    +-----------+
    |  Donnee 2 |
    +-----------+
    |  Donnee 1 |
    +-----------+
    |   ....    |
Adresses basses
```

### Cortex-M0 : Full-Descending Stack

- **SP (R13)** pointe vers le **dernier element empile**
- La pile **croit vers les adresses basses**
- **PUSH** : decremente SP puis ecrit
- **POP** : lit puis incremente SP

```
PUSH :  SP = SP - 4,  Mem[SP] = Registre
POP  :  Registre = Mem[SP],  SP = SP + 4
```

> Le SP est initialise par le code de demarrage avec la valeur stockee a l'adresse 0x00000000 (premier mot de la table des vecteurs).

---

## 9.2 Operations PUSH et POP

```asm
// Sauvegarder R4-R7 et LR sur la pile
push {r4-r7, lr}

// Avant : SP = 0x20002000
// Apres : SP = 0x20001FEC (5 mots × 4 = 20 octets)
//
// 0x20001FEC : R4
// 0x20001FF0 : R5
// 0x20001FF4 : R6
// 0x20001FF8 : R7
// 0x20001FFC : LR
// 0x20002000 : (ancien sommet)

// Restaurer
pop {r4-r7, pc}       // LR → PC (retour automatique)
// SP revient a 0x20002000
```

---

## 9.3 Cadre de Pile (Stack Frame)

Chaque appel de fonction cree un **cadre de pile** contenant :

```
Adresses hautes
+---------------------------+
| Parametres supplementaires|  (passes par l'appelant, si > 4)
+---------------------------+
| LR sauvegarde             |  ← empile par PUSH
+---------------------------+
| Registres sauvegardes     |  R4-R7 (callee-saved)
| (R4, R5, R6, R7)          |
+---------------------------+
| Variables locales         |  ← allouees par SUB SP
| (var1, var2, ...)         |
+---------------------------+ ← SP actuel
```

---

## 9.4 Variables Locales

Les variables locales sont allouees sur la pile en **soustrayant** de SP, et liberees en **ajoutant** a SP.

```asm
func:
    push {r4-r7, lr}         // sauvegarder registres

    sub sp, sp, #12          // allouer 3 mots (12 octets)
                              // var1 = [SP+0], var2 = [SP+4], var3 = [SP+8]

    // Utiliser les variables locales
    movs r0, #42
    str r0, [sp, #0]          // var1 = 42
    movs r0, #100
    str r0, [sp, #4]          // var2 = 100
    ldr r1, [sp, #0]          // r1 = var1

    add sp, sp, #12           // liberer les variables locales
    pop {r4-r7, pc}           // restaurer et retourner
```

### Diagramme de la pile

```
Avant l'appel :     SP → |           |  0x20002000

Apres PUSH :        SP → | R4        |  0x20001FEC
                          | R5        |  0x20001FF0
                          | R6        |  0x20001FF4
                          | R7        |  0x20001FF8
                          | LR        |  0x20001FFC

Apres SUB SP, #12 : SP → | var1      |  0x20001FE0
                          | var2      |  0x20001FE4
                          | var3      |  0x20001FE8
                          | R4        |  0x20001FEC
                          | ...       |
```

---

## 9.5 Passage de Parametres par la Pile

Quand une fonction a **plus de 4 parametres** :

```asm
// Appel : func(1, 2, 3, 4, 5, 6)
// R0=1, R1=2, R2=3, R3=4, pile: 5 et 6

movs r0, #6
push {r0}             // empiler 6eme parametre
movs r0, #5
push {r0}             // empiler 5eme parametre
movs r0, #1           // 1er parametre
movs r1, #2           // 2eme
movs r2, #3           // 3eme
movs r3, #4           // 4eme
bl func

add sp, sp, #8        // nettoyer la pile (2 mots empiles)
```

```asm
// Dans la fonction appelee :
func:
    push {r4, lr}
    // Parametres 5 et 6 sont au-dessus du cadre :
    ldr r4, [sp, #8]      // 5eme parametre (SP + 2 mots sauvegardes)
    ldr r5, [sp, #12]     // 6eme parametre
    ...
    pop {r4, pc}
```

---

## 9.6 Exemple Complet

```asm
// Fonction : int sum_and_diff(int a, int b, int *diff)
// Retourne a+b, stocke a-b dans *diff
// R0=a, R1=b, R2=pointeur vers diff

sum_and_diff:
    push {r4, lr}          // PROLOGUE
    sub sp, sp, #4         // variable locale temp (4 octets)

    adds r4, r0, r1        // r4 = a + b (somme)
    subs r3, r0, r1        // r3 = a - b (difference)
    str r3, [sp, #0]       // temp = difference (variable locale)

    ldr r3, [sp, #0]       // relire temp
    str r3, [r2]           // *diff = difference

    mov r0, r4             // retourner somme dans r0

    add sp, sp, #4         // liberer variable locale
    pop {r4, pc}           // EPILOGUE : restaurer et retourner
```

---

## Resume

- **Pile** : structure LIFO, croit vers les adresses basses (full-descending)
- **PUSH** : SP -= 4 puis ecriture | **POP** : lecture puis SP += 4
- **Variables locales** : `SUB SP, #N` pour allouer, `ADD SP, #N` pour liberer
- **Acces** : `LDR/STR Rt, [SP, #offset]`
- **Cadre de pile** : registres sauvegardes + variables locales + parametres
- **Parametres > 4** : passes via la pile par l'appelant

---

[<< Chapitre 8](./08-Sous-Programmes.md) | [Chapitre 10 : Modele d'Exception >>](./10-Modele-Exception.md)
