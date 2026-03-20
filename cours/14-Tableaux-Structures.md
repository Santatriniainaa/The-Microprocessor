# Chapitre 14 : Tableaux et Structures

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 13](./13-Fonctions-C.md) | [Chapitre 15 >>](./15-Fonctions-Avancees-C.md)

---

## Objectifs

> Maitriser les tableaux, structures, unions, champs de bits et pointeurs en C sur ARM.

---

## 14.1 Tableaux

### Declaration et stockage en memoire

```c
uint32_t arr[5] = {10, 20, 30, 40, 50};
```

```
Adresse base (arr) :
+------+------+------+------+------+
| 10   | 20   | 30   | 40   | 50   |
+------+------+------+------+------+
arr+0  arr+4  arr+8  arr+12 arr+16
[0]    [1]    [2]    [3]    [4]
```

### Acces par indice

**Formule** : `adresse = base + index × taille_element`

```c
x = arr[3];    // adresse = arr + 3 * 4 = arr + 12
```

```asm
ldr r0, =arr          // r0 = adresse de base
ldr r1, [r0, #12]     // r1 = arr[3] (offset = 3 × 4)
```

### Parcours avec index variable

```c
for (int i = 0; i < 5; i++) {
    sum += arr[i];
}
```

```asm
    ldr r0, =arr
    movs r2, #0           // i = 0
    movs r3, #0           // sum = 0
loop:
    cmp r2, #5
    bge done
    lsls r4, r2, #2       // r4 = i * 4
    ldr r5, [r0, r4]      // r5 = arr[i]
    adds r3, r3, r5       // sum += arr[i]
    adds r2, #1
    b loop
done:
```

---

## 14.2 Chaines de Caracteres

```c
char str[] = "Hello";   // {'H','e','l','l','o','\0'} → 6 octets
```

```
+-----+-----+-----+-----+-----+-----+
| 'H' | 'e' | 'l' | 'l' | 'o' | '\0'|
| 0x48| 0x65| 0x6C| 0x6C| 0x6F| 0x00|
+-----+-----+-----+-----+-----+-----+
```

---

## 14.3 Structures

```c
typedef struct {
    uint8_t  id;          // 1 octet
    // 3 octets de padding (alignement)
    uint32_t value;       // 4 octets
    uint16_t flags;       // 2 octets
    // 2 octets de padding
} Sensor;                 // sizeof = 12 octets (pas 7 !)
```

### Stockage en memoire avec padding

```
Offset  Contenu        Taille
+0      id             1 octet
+1      (padding)      3 octets     ← alignement pour value (4 octets)
+4      value          4 octets
+8      flags          2 octets
+10     (padding)      2 octets     ← alignement structure
= 12 octets total
```

### Acces aux champs

```c
Sensor s;
s.value = 100;    // Accede a l'offset +4
```

```asm
ldr r0, =s
movs r1, #100
str r1, [r0, #4]      // offset de value = 4
```

---

## 14.4 Unions

```c
typedef union {
    uint32_t word;        // 4 octets
    uint16_t half[2];     // 2 × 2 octets
    uint8_t  byte[4];     // 4 × 1 octet
} DataView;               // sizeof = 4 (taille du plus grand membre)
```

Tous les membres **partagent la meme memoire** :

```c
DataView d;
d.word = 0x12345678;
// d.half[0] = 0x5678  (little-endian)
// d.half[1] = 0x1234
// d.byte[0] = 0x78
// d.byte[3] = 0x12
```

---

## 14.5 Champs de Bits

Utiles pour mapper les **registres de peripheriques** :

```c
typedef struct {
    uint32_t ENABLE  : 1;    // bit 0
    uint32_t TICKINT : 1;    // bit 1
    uint32_t CLKSRC  : 1;    // bit 2
    uint32_t         : 13;   // bits 3-15 reserves
    uint32_t COUNTFLAG : 1;  // bit 16
    uint32_t         : 15;   // bits 17-31 reserves
} SysTick_CSR_t;
```

---

## 14.6 Pointeurs

### Concept

```c
uint32_t x = 42;
uint32_t *ptr = &x;     // ptr contient l'adresse de x

*ptr = 100;              // x vaut maintenant 100
uint32_t y = *ptr;       // y = 100
```

### Arithmetique de pointeurs

```c
uint32_t arr[5] = {10, 20, 30, 40, 50};
uint32_t *p = arr;       // p pointe vers arr[0]

p++;                      // p pointe vers arr[1] (avance de 4 octets)
*(p + 2) = 99;           // arr[3] = 99
```

### Pointeurs et structures

```c
Sensor *ps = &sensor1;
ps->value = 200;         // equivalent a (*ps).value = 200
```

```asm
ldr r0, =sensor1         // r0 = adresse de la structure
movs r1, #200
str r1, [r0, #4]         // ps->value (offset 4)
```

---

## Resume

- **Tableaux** : elements contigus, acces = base + index × taille
- **Structures** : padding pour l'alignement, acces par offset fixe
- **Unions** : membres partagent la memoire
- **Champs de bits** : pour mapper les registres de peripheriques
- **Pointeurs** : variables contenant des adresses, arithmetique par taille de type

---

[<< Chapitre 13](./13-Fonctions-C.md) | [Chapitre 15 : Fonctions Avancees en C >>](./15-Fonctions-Avancees-C.md)
