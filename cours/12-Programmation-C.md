# Chapitre 12 : Programmation en C

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 11](./11-Interruptions-SysTick.md) | [Chapitre 13 >>](./13-Fonctions-C.md)

---

## Objectifs

> Programmer le Cortex-M0 en langage C : types de donnees, variables, operateurs, structures de controle, et correspondance avec l'assembleur ARM.

---

## 12.1 Pourquoi le C ?

| Critere | Assembleur | C |
|---------|-----------|---|
| Portabilite | Aucune | Elevee |
| Productivite | Faible | Elevee |
| Controle bas niveau | Total | Bon (pointeurs, volatile) |
| Performance | Optimale | Proche de l'optimal |
| Lisibilite | Difficile | Bonne |

---

## 12.2 Types de Donnees

### Types standards et types a taille fixe

| Type C | stdint.h | Taille | Instructions ARM |
|--------|----------|--------|------------------|
| `char` | `uint8_t` / `int8_t` | 8 bits | `LDRB`/`STRB` |
| `short` | `uint16_t` / `int16_t` | 16 bits | `LDRH`/`STRH` |
| `int` | `uint32_t` / `int32_t` | 32 bits | `LDR`/`STR` |
| `long` | `uint32_t` / `int32_t` | 32 bits | `LDR`/`STR` |
| `long long` | `uint64_t` / `int64_t` | 64 bits | 2× `LDR`/`STR` |

> **Recommandation** : Utiliser les types `<stdint.h>` pour la clarte et la portabilite.

---

## 12.3 Variables

### Stockage en memoire

| Type de variable | Section | Memoire | Duree de vie |
|-----------------|---------|---------|-------------|
| Globale initialisee | `.data` | RAM | Programme entier |
| Globale non init. | `.bss` | RAM | Programme entier |
| Globale constante | `.rodata` | ROM | Programme entier |
| Locale | Pile/Registres | RAM | Duree de la fonction |
| Locale static | `.data`/`.bss` | RAM | Programme entier |

### Qualificateurs importants

```c
volatile uint32_t *reg = (volatile uint32_t *)0x40000000;
// volatile : empeche le compilateur d'optimiser les acces
// Indispensable pour : registres E/S, variables partagees avec ISR

const uint32_t table[] = {1, 2, 3, 4};
// const : stocke en ROM (Flash), economise la RAM

static int count = 0;
// static dans une fonction : persiste entre les appels
// static au niveau fichier : portee limitee au fichier
```

---

## 12.4 Operateurs et Correspondance ARM

### Arithmetiques

| Operateur C | Instruction ARM | Exemple |
|------------|----------------|---------|
| `a + b` | `ADDS Rd, Rn, Rm` | `r0 = r1 + r2` |
| `a - b` | `SUBS Rd, Rn, Rm` | `r0 = r1 - r2` |
| `a * b` | `MULS Rd, Rn, Rd` | `r0 = r1 * r0` |
| `a / b` | Routine logicielle | (pas d'instruction DIV) |
| `a % b` | Routine logicielle | (pas d'instruction MOD) |

### Logiques bit a bit

| Operateur C | Instruction ARM | Exemple |
|------------|----------------|---------|
| `a & b` | `ANDS Rdn, Rm` | AND bit a bit |
| `a \| b` | `ORRS Rdn, Rm` | OR bit a bit |
| `a ^ b` | `EORS Rdn, Rm` | XOR bit a bit |
| `~a` | `MVNS Rd, Rm` | NOT bit a bit |
| `a << n` | `LSLS Rd, Rm, #n` | Decalage gauche |
| `a >> n` | `LSRS`/`ASRS` | Decalage droite |

### Comparaison

| Operateur C | Instruction ARM |
|------------|----------------|
| `a == b` | `CMP` + `BEQ` |
| `a != b` | `CMP` + `BNE` |
| `a < b` | `CMP` + `BLT` (signe) / `BLO` (non signe) |
| `a > b` | `CMP` + `BGT` (signe) / `BHI` (non signe) |

---

## 12.5 Structures de Controle

### if / else

```c
if (x > 10) {          //     cmp r0, #10
    y = 1;              //     ble else_label
} else {                //     movs r1, #1     // y = 1
    y = 0;              //     b end_label
}                       // else_label:
                        //     movs r1, #0     // y = 0
                        // end_label:
```

### while

```c
while (x > 0) {         // loop:
    x--;                 //     cmp r0, #0
    sum += x;            //     ble end_loop
}                        //     subs r0, #1
                         //     adds r1, r1, r0
                         //     b loop
                         // end_loop:
```

### for

```c
for (int i = 0;          //     movs r0, #0        // i = 0
     i < 10;             // loop:
     i++) {              //     cmp r0, #10
    sum += i;            //     bge end_loop
}                        //     adds r1, r1, r0   // sum += i
                         //     adds r0, #1       // i++
                         //     b loop
                         // end_loop:
```

### switch/case

Le compilateur peut generer soit des comparaisons en serie, soit une **table de branchement** (pour les cas consecutifs).

---

## 12.6 Compilation C vers Assembleur

Le compilateur transforme le code C en assembleur ARM puis en code machine.

### Niveaux d'optimisation

| Niveau | Description | Debogage |
|--------|-------------|----------|
| `-O0` | Aucune optimisation | Facile |
| `-O1` | Optimisations basiques | Acceptable |
| `-O2` | Optimisations avancees | Difficile |
| `-O3` | Optimisations agressives | Tres difficile |
| `-Os` | Optimiser la taille | Variable |

> Pour le developpement/debogage : `-O0`. Pour la production : `-O2` ou `-Os`.

---

## Resume

- **Types** : utiliser `stdint.h` (`uint8_t`, `int32_t`, etc.)
- **volatile** : obligatoire pour registres E/S et variables partagees ISR
- **const** : place les donnees en ROM
- **Operateurs** C correspondent directement aux instructions ARM
- **Structures de controle** : `CMP` + branchements conditionnels
- Compilation avec optimisations : `-O0` (debug) a `-O2` (production)

---

[<< Chapitre 11](./11-Interruptions-SysTick.md) | [Chapitre 13 : Fonctions en C >>](./13-Fonctions-C.md)
