# Chapitre 16 : Processus de Compilation

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 15](./15-Fonctions-Avancees-C.md) | [Chapitre 17 >>](./17-Microcontroleur.md)

---

## Objectifs

> Comprendre la chaine d'outils de compilation : du code source au programme executable charge en memoire.

---

## 16.1 Chaine d'Outils (Toolchain)

```
  Code C (.c)     Code ASM (.s)
      |                |
      v                |
 +-----------+         |
 |Preprocesseur|       |
 +-----------+         |
      |                |
      v                |
 +-----------+         |
 |Compilateur|         |
 +-----------+         |
      |                |
      v                v
 +-----------+   +-----------+
 |Assembleur |   |Assembleur |
 +-----------+   +-----------+
      |                |
      v                v
   Objet (.o)      Objet (.o)     Bibliotheques (.a)
      |                |                |
      +--------+-------+--------+-------+
               |
               v
        +-----------+
        |  Linker   |
        | (Editeur  |
        |  de liens)|
        +-----------+
               |
               v
        Executable (.axf)
               |
               v
         Fichier .hex/.bin
         (chargé en Flash)
```

### Outils ARM

| Outil | Role |
|-------|------|
| `armclang` | Preprocesseur + Compilateur + Assembleur |
| `armlink` | Editeur de liens |
| `fromelf` | Conversion .axf → .hex/.bin |

---

## 16.2 Preprocesseur

Transforme le code C avant compilation :

| Directive | Action |
|-----------|--------|
| `#include` | Insere le contenu d'un fichier |
| `#define` | Definit une macro/constante |
| `#ifdef/#endif` | Compilation conditionnelle |

---

## 16.3 Compilateur

Convertit le C en assembleur ARM :
- Analyse syntaxique et semantique
- Optimisations selon le niveau (`-O0` a `-O3`)
- Generation de code assembleur

---

## 16.4 Assembleur

Convertit l'assembleur en **fichier objet** (.o) au format **ELF** :

| Contenu du .o | Description |
|---------------|-------------|
| Sections (.text, .data, .bss, .rodata) | Code et donnees |
| Table des symboles | Noms des fonctions et variables |
| Table de relocation | Adresses a resoudre par le linker |

---

## 16.5 Editeur de Liens (Linker)

### Role

1. **Combine** tous les fichiers objet + bibliotheques
2. **Resout** les symboles externes (relie les appels aux definitions)
3. **Place** les sections en memoire selon le **scatter file**
4. Genere l'**executable** (.axf)

### Scatter File (fichier de placement memoire)

```
LR_IROM1 0x00000000 0x00020000 {   ; ROM 128 kB
    ER_IROM1 0x00000000 0x00020000 {
        *.o (RESET, +First)          ; Table des vecteurs en premier
        *(InRoot$$Sections)
        .ANY (+RO)                   ; Code + constantes
    }
    RW_IRAM1 0x20000000 0x00002000 { ; RAM 8 kB
        .ANY (+RW +ZI)              ; Variables
    }
}
```

---

## 16.6 Carte Memoire du Programme

```
ROM (Flash) 0x00000000               RAM (SRAM) 0x20000000
+---------------------+              +--------------------+
| Table des vecteurs  |              | .data              |
| (.vectors)          |              | (copie depuis ROM) |
+---------------------+              +--------------------+
| .text               |              | .bss               |
| (code)              |              | (initialise a 0)   |
+---------------------+              +--------------------+
| .rodata             |              | Tas (heap) ↑       |
| (constantes)        |              +--------------------+
+---------------------+              | (espace libre)     |
| .data (init values) |              +--------------------+
| (valeurs initiales) |              | Pile (stack) ↓     |
+---------------------+              +--------------------+
```

### Code de demarrage (Startup)

```
1. SP ← valeur a l'adresse 0x00000000
2. PC ← Reset_Handler (adresse a 0x00000004)
3. Reset_Handler :
   a. Copie .data de ROM vers RAM
   b. Initialise .bss a 0 en RAM
   c. Appelle pre_main → main (ou asm_main)
```

### Fichier .map

Le linker genere un fichier `.map` qui montre :
- Adresses et tailles de chaque section
- Placement de chaque symbole
- Utilisation memoire ROM/RAM

---

## Resume

- **Chaine** : Preprocesseur → Compilateur → Assembleur → Linker
- **Objet .o** : code machine + symboles + relocations (format ELF)
- **Linker** : combine, resout symboles, place en memoire (scatter file)
- **Memoire** : ROM = code + constantes + init .data | RAM = .data + .bss + pile + tas
- **Startup** : copie .data, initialise .bss, appelle main

---

[<< Chapitre 15](./15-Fonctions-Avancees-C.md) | [Chapitre 17 : Le Microcontroleur >>](./17-Microcontroleur.md)
