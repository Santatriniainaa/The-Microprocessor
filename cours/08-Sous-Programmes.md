# Chapitre 8 : Sous-Programmes et Appels de Fonctions

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 7](./07-Acces-Memoire.md) | [Chapitre 9 >>](./09-Pile-Variables-Locales.md)

---

## Objectifs

> Comprendre le mecanisme d'appel/retour de fonctions, la convention AAPCS, et la sauvegarde des registres.

---

## 8.1 Concept de Sous-Programme

Un sous-programme (fonction) est un bloc de code **reutilisable** appele depuis differents endroits du programme.

```
Programme principal          Sous-programme
─────────────────            ───────────────
    ...                      func1:
    BL func1  ─────────→        ...
    (suite)   ←─────────        BX LR (retour)
    ...
    BL func1  ─────────→        ...
    (suite)   ←─────────        BX LR (retour)
```

---

## 8.2 Instructions d'Appel et Retour

| Instruction | Action | Portee |
|------------|--------|--------|
| `BL <label>` | LR ← adresse retour, PC ← label | ±16 Mo (32 bits) |
| `BLX <Rm>` | LR ← adresse retour, PC ← Rm | N'importe ou (32 bits) |
| `BX LR` | PC ← LR (retour) | - |

### Mecanisme

```
Avant BL :   PC = 0x200 (adresse de BL func1)
Apres BL :   LR = 0x204 (adresse retour = instruction suivante)
             PC = adresse de func1

A la fin de func1 :
BX LR :      PC ← LR = 0x204 (retour a l'appelant)
```

---

## 8.3 Convention AAPCS (ARM Architecture Procedure Call Standard)

### Passage de parametres

| Registre | Role |
|----------|------|
| **R0** | 1er parametre / valeur de retour |
| **R1** | 2eme parametre (ou retour 64 bits avec R0) |
| **R2** | 3eme parametre |
| **R3** | 4eme parametre |
| **Pile** | 5eme parametre et suivants |

### Sauvegarde des registres

| Registres | Sauvegarde par | Nom |
|-----------|---------------|-----|
| **R0-R3, R12** | **Appelant** (caller-saved) | Registres scratch |
| **R4-R11** | **Appele** (callee-saved) | Registres preserves |
| **LR (R14)** | **Appele** (si appels imbriques) | Registre de lien |
| **SP (R13)** | Doit etre restaure | Pointeur de pile |

> **Regle** : Si une fonction modifie R4-R11, elle **doit** les sauvegarder au debut et les restaurer a la fin.

---

## 8.4 Prologue et Epilogue

### Fonction simple (sans appels imbriques)

```asm
func_simple:
    // Pas besoin de sauvegarder LR (pas d'appel interne)
    adds r0, r0, r1       // calcul
    bx lr                  // retour
```

### Fonction avec appels imbriques

```asm
func_complexe:
    // PROLOGUE : sauvegarder les registres utilises + LR
    push {r4-r7, lr}

    // Corps de la fonction
    mov r4, r0             // sauvegarder parametre
    bl autre_func          // appel imbrique (ecrase LR !)
    adds r0, r4, r0        // utiliser resultat

    // EPILOGUE : restaurer et retourner
    pop {r4-r7, pc}        // pop LR directement dans PC = retour
```

> **Astuce** : `POP {R4-R7, PC}` restaure les registres **et** retourne en une seule instruction (LR sauvegarde est charge dans PC).

---

## 8.5 Appels Imbriques

```
main → BL funcA → BL funcB → BX LR → BX LR

Sans sauvegarde de LR :
  BL funcA : LR = retour_main
  BL funcB : LR = retour_funcA  ← LR ecrase ! retour_main perdu !

Avec sauvegarde de LR sur la pile :
  funcA:
    PUSH {LR}            // sauvegarde retour_main
    BL funcB              // LR = retour_funcA (OK)
    POP {PC}              // restaure retour_main dans PC → retour
```

---

## 8.6 Exemple Complet

```asm
// Fonction : result = multiply_add(a, b, c) = a*b + c
// Parametres : R0=a, R1=b, R2=c
// Retour : R0 = resultat
multiply_add:
    muls r0, r1, r0       // r0 = a * b
    adds r0, r0, r2       // r0 = a*b + c
    bx lr                  // retour avec resultat dans r0

// Programme principal
asm_main:
    push {lr}

    movs r0, #7           // a = 7
    movs r1, #10          // b = 10
    movs r2, #11          // c = 11
    bl multiply_add        // r0 = 7*10 + 11 = 81

    pop {pc}               // retour
```

### Alignement de la pile

> La pile doit etre alignee sur **8 octets** au moment d'un appel de fonction (convention AAPCS). Si necessaire, ajouter un registre supplementaire au PUSH.

```asm
push {r4-r6, lr}          // 4 registres × 4 = 16 octets (aligne 8) ✓
push {r4-r7, lr}          // 5 registres × 4 = 20 octets (pas aligne 8)
                           // Ajouter un registre : push {r4-r7, lr} + sub sp, #4
```

---

## Resume

- **BL** : appel (sauvegarde retour dans LR), **BX LR** : retour
- **AAPCS** : R0-R3 = parametres, R0 = retour, R4-R11 = preserves
- **Prologue** : `PUSH {R4-Rx, LR}` — **Epilogue** : `POP {R4-Rx, PC}`
- Appels imbriques : **toujours** sauvegarder LR sur la pile
- Pile alignee sur **8 octets** avant un appel

---

[<< Chapitre 7](./07-Acces-Memoire.md) | [Chapitre 9 : Pile et Variables Locales >>](./09-Pile-Variables-Locales.md)
