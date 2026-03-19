# Chapitre 4 : Programmation en Assembleur

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 3](./03-Execution-Programme.md) | [Chapitre 5 >>](./05-Operations-Arithmetiques.md)

---

## Objectifs

> Apprendre a ecrire des programmes en langage assembleur ARM : syntaxe, sections, directives, et exercices pratiques avec Keil uVision.

---

## 4.1 Types d'Instructions (Statements)

Un programme assembleur est compose de 4 types d'instructions :

| Type | Role | Exemple |
|------|------|---------|
| **Commentaires** | Ameliorent la lisibilite | `// ceci est un commentaire` |
| **Labels** | Nom symbolique pour une adresse | `asm_main:` |
| **Instructions** | Operations executees par le processeur | `adds r0, r1, r2` |
| **Directives** | Informations pour l'assembleur | `.word 0x12345678` |

### 4.1.1 Commentaires

```asm
// Commentaire sur une seule ligne

/* Commentaire
   sur plusieurs
   lignes */

muls r0, r1, r0    // compute m*x
add r0, r2          // add c, result m*x + c
```

### 4.1.2 Espaces et sensibilite a la casse

- **Instructions et directives** : insensibles a la casse (`ADD` = `add` = `Add`)
- **Labels** : **sensibles** a la casse (`Label1` ≠ `label1` ≠ `LABEL1`)
- Espaces et tabulations sont ignores (formatage libre)
- Chaque instruction sur une **nouvelle ligne** (ou separee par `;`)

---

## 4.2 Labels (Etiquettes)

Un label est un **nom symbolique** associe a une adresse memoire.

### Utilisations

```asm
// Label d'instruction (pour branchement)
L1: adds r0, r1, r2

// Label de fonction
asm_main:
    <instructions>

// Label de donnee
x:  .word 0x12345678
```

### Acces aux donnees via labels

```asm
x:    .word 0x12345678     // definir x = 0x12345678

      ldr r0, =x           // charger l'adresse de x dans r0
      ldr r1, [r0]         // charger la valeur de x dans r1
      str r1, [r0]         // stocker r1 a l'adresse de x
```

### Portee des labels

| Portee | Declaration | Visibilite |
|--------|------------|------------|
| **Locale** | `label:` (par defaut) | Fichier courant uniquement |
| **Globale** | `.global label` | Tous les fichiers du programme |

> En C, la portee est locale a la **fonction**. En assembleur, elle est locale au **fichier**.

---

## 4.3 Sections

Un programme assembleur est organise en **sections** :

```
+-------------------+
| .text             |  Instructions (code)
| (codes instruction)|
+-------------------+
| .rodata           |  Donnees en lecture seule (constantes)
| (constantes)      |
+-------------------+
| .data             |  Donnees initialisees (variables)
| (var. initialisees)|
+-------------------+
| .bss              |  Donnees non initialisees
| (var. non init.)  |
+-------------------+
```

| Section | Contenu | Stockage | Modifiable |
|---------|---------|----------|------------|
| `.text` | Instructions du programme | ROM (Flash) | Non |
| `.rodata` | Constantes | ROM (Flash) | Non |
| `.data` | Variables initialisees | ROM → copie en RAM | Oui |
| `.bss` | Variables non initialisees | RAM (init a 0) | Oui |

> **Format ELF** (Executable and Linkable Format) : Standard pour les fichiers objet, bibliotheques et executables ARM.

### Sections speciales

| Section | Contenu |
|---------|---------|
| `.vectors` | Table des vecteurs (debut du programme) |
| `.stack` | Espace reserve pour la pile |

---

## 4.4 Section .text (Code)

### Structure d'une fonction

```asm
    .text                    // Section de code
    .balign 2                // Alignement sur 2 octets (Thumb)

    .global asm_main         // Rendre le symbole global
    .thumb_func              // Fonction en mode Thumb
asm_main:                    // Label de la fonction
    // instructions ici

    .size asm_main,.-asm_main   // Taille de la fonction
```

### Code de demarrage (Startup)

Au demarrage du processeur :

```
1. Table des vecteurs (adresse 0x00000000)
   - Mot 0 : Valeur initiale du SP
   - Mot 1 : Adresse du Reset_Handler

2. Reset_Handler
   → Appelle pre_main (initialise .data et .bss)
   → Appelle asm_main (notre programme)

3. asm_main
   → Notre code s'execute ici
```

### Exercice 4.1 : Premier programme assembleur

```asm
    .syntax unified          // Syntaxe UAL

    .text
    .balign 2

    .global asm_main
    .thumb_func
asm_main:
    // Addition de deux nombres
    movs r0, #22             // premier nombre
    movs r1, #33             // second nombre
    adds r2, r0, r1          // r2 = r0 + r1 = 55

L0: b L0                     // boucle infinie (fin)
    .size asm_main,.-asm_main

    .end
```

**Resultat apres execution :**
- R0 = 22 (0x16)
- R1 = 33 (0x21)
- R2 = 55 (0x37)

---

## 4.5 Sections de Donnees

### 4.5.1 Donnees en lecture seule (.rodata)

```asm
    .rodata                  // Section constantes

    .global x1
x1: .byte 0x11              // Octet (8 bits)

    .global x2
    .balign 2
x2: .hword 0x2222           // Demi-mot (16 bits)

    .global x3
    .balign 4
x3: .word 0x44444444        // Mot (32 bits)
```

### Directives de definition de donnees

| Directive | Taille | Exemple |
|-----------|--------|---------|
| `.byte` | 8 bits | `.byte 0xAA, 0xBB, 0xCC` |
| `.hword` | 16 bits | `.hword 0x1234, 0x5678` |
| `.word` | 32 bits | `.word 0x12345678` |
| `.ascii` | Chaine (sans \0) | `.ascii "abcd"` |
| `.string` | Chaine (avec \0) | `.string "hello"` |

### 4.5.2 Donnees initialisees (.data)

Stockees en ROM, copiees en **RAM** par le code de demarrage.

```asm
    .data                    // Section variables initialisees

    .global count
    .balign 4
count: .word 10             // count = 10

    .global a
    .balign 2
a:  .hword 0x1111, 0x2222, 0x4444   // tableau de 3 demi-mots
```

### 4.5.3 Donnees non initialisees (.bss)

En RAM, initialisees a **0** par le code de demarrage.

```asm
    .bss                     // Section variables non initialisees

    .global y
    .balign 4
y:  .space 4                // Alloue 4 octets pour y

    .global arr
    .balign 4
arr: .space 40              // Alloue 10 mots (40 octets)
```

### Alignement

| Donnee | Directive | Alignement |
|--------|-----------|------------|
| Octet | `.balign 1` (ou rien) | Aucune contrainte |
| Demi-mot | `.balign 2` | Adresse multiple de 2 |
| Mot | `.balign 4` | Adresse multiple de 4 |

> **Important** : Un acces non aligne sur Cortex-M0 provoque un **HardFault** !

---

### Exercice 4.3 : Programme complet y = m*x + c

```asm
    .syntax unified

    // ===== Constantes (ROM) =====
    .rodata
    .global m
    .balign 4
m:  .word 7                 // m = 7

    .global c
    .balign 4
c:  .word 11                // c = 11

    // ===== Variables initialisees (RAM) =====
    .data
    .global x
    .balign 4
x:  .word 10                // x = 10

    // ===== Variables non initialisees (RAM) =====
    .bss
    .global y
    .balign 4
y:  .space 4                // y (resultat)

    // ===== Code =====
    .text
    .balign 2

    .global asm_main
    .thumb_func
asm_main:
    ldr r3, =x              // adresse de x → r3
    ldr r0, [r3]            // valeur de x → r0

    ldr r3, =m              // adresse de m → r3
    ldr r1, [r3]            // valeur de m → r1

    ldr r3, =c              // adresse de c → r3
    ldr r2, [r3]            // valeur de c → r2

    muls r0, r1, r0         // r0 = m * x = 70
    adds r0, r0, r2         // r0 = m*x + c = 81

    ldr r3, =y              // adresse de y → r3
    str r0, [r3]            // stocker resultat dans y

L0: b L0                    // boucle infinie
    .size asm_main,.-asm_main
    .end
```

> La pseudo-instruction `ldr r0, =x` charge l'**adresse** du symbole x. L'assembleur la remplace par `ldr r0, [pc, #offset]` + un litteral en memoire.

---

## 4.6 Resume

### Statements
- 4 types : commentaires, labels, instructions, directives
- Labels sensibles a la casse, portee locale par defaut (.global pour globale)

### Sections
- `.text` : instructions (ROM)
- `.rodata` : constantes (ROM)
- `.data` : variables initialisees (ROM → RAM)
- `.bss` : variables non initialisees (RAM, init a 0)

### Directives de donnees
- `.word` (32 bits), `.hword` (16 bits), `.byte` (8 bits)
- `.space N` pour reserver N octets non initialises
- `.balign N` pour l'alignement

### Structure du programme
- Code de demarrage → Reset_Handler → pre_main → asm_main
- Fonction : `.global` + `.thumb_func` + `label:`

---

[<< Chapitre 3](./03-Execution-Programme.md) | [Chapitre 5 : Operations Arithmetiques >>](./05-Operations-Arithmetiques.md)
