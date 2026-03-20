# Chapitre 15 : Fonctions Avancees en C

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 14](./14-Tableaux-Structures.md) | [Chapitre 16 >>](./16-Processus-Compilation.md)

---

## Objectifs

> Maitriser les concepts avances du C pour l'embarque : pointeurs de fonctions, preprocesseur, qualificateurs, acces aux registres materiels.

---

## 15.1 Pointeurs de Fonctions

```c
// Declaration
int (*operation)(int, int);

// Affectation
operation = add;        // pointe vers la fonction add

// Appel
int result = operation(3, 4);  // appelle add(3, 4)
```

### Callbacks

```c
void configure_timer(void (*callback)(void)) {
    // ... configuration ...
    timer_callback = callback;
}

void on_timer_expire(void) {
    toggle_led();
}

// Utilisation
configure_timer(on_timer_expire);
```

### Table de pointeurs de fonctions

```c
typedef void (*handler_t)(void);
handler_t handlers[] = {handler_0, handler_1, handler_2};

// Appel par index
handlers[event_id]();
```

---

## 15.2 Preprocesseur

### Macros et constantes

```c
#define LED_PIN     5
#define BIT(n)      (1U << (n))
#define SET_BIT(reg, n)    ((reg) |= BIT(n))
#define CLR_BIT(reg, n)    ((reg) &= ~BIT(n))
#define READ_BIT(reg, n)   (((reg) >> (n)) & 1U)
```

### Compilation conditionnelle

```c
#ifdef DEBUG
    printf("x = %d\n", x);
#endif

#ifndef HEADER_H
#define HEADER_H
// contenu du header
#endif
```

---

## 15.3 Typedef et Enum

```c
typedef uint32_t reg32_t;
typedef void (*irq_handler_t)(void);

typedef enum {
    GPIO_MODE_INPUT  = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_ALT    = 2,
    GPIO_MODE_ANALOG = 3
} gpio_mode_t;
```

---

## 15.4 Allocation Dynamique

```c
#include <stdlib.h>

uint8_t *buffer = (uint8_t *)malloc(256);
if (buffer != NULL) {
    // utiliser buffer
    free(buffer);
}
```

### Organisation memoire

```
Adresses hautes
+------------------+
|      Pile ↓      |  Croit vers le bas
+------------------+
|    (espace libre)|
+------------------+
|      Tas ↑       |  Croit vers le haut (malloc)
+------------------+
|      .bss        |  Variables non initialisees
+------------------+
|      .data       |  Variables initialisees
+------------------+
Adresses basses (RAM)
```

> **En embarque** : eviter `malloc`/`free` si possible (fragmentation, imprevisibilite). Preferer les allocations statiques.

---

## 15.5 Qualificateurs pour l'Embarque

### volatile

```c
// OBLIGATOIRE pour :
// 1. Registres de peripheriques
volatile uint32_t *GPIO_DATA = (volatile uint32_t *)0x400043FC;

// 2. Variables modifiees par une ISR
volatile uint32_t tick_count = 0;

void SysTick_Handler(void) {
    tick_count++;    // modifiee dans l'ISR
}

void delay_ms(uint32_t ms) {
    uint32_t start = tick_count;
    while ((tick_count - start) < ms);  // lu dans la boucle
    // Sans volatile, le compilateur pourrait optimiser cette boucle !
}
```

### const

```c
const uint32_t lookup_table[] = {0, 1, 4, 9, 16, 25};
// Stocke en ROM (Flash) → economise la RAM
```

### static

```c
// Dans une fonction : variable persistante
void count_calls(void) {
    static int count = 0;   // conserve entre les appels
    count++;
}

// Au niveau fichier : portee limitee au fichier
static int module_state = 0;  // invisible depuis les autres fichiers
```

---

## 15.6 Acces aux Registres Materiels en C

### Methode 1 : Pointeurs directs

```c
#define GPIOA_DIR   (*(volatile uint32_t *)0x40004400)
#define GPIOA_DATA  (*(volatile uint32_t *)0x400043FC)

GPIOA_DIR |= (1 << 5);     // configurer pin 5 en sortie
GPIOA_DATA |= (1 << 5);    // activer pin 5 (LED ON)
GPIOA_DATA &= ~(1 << 5);   // desactiver pin 5 (LED OFF)
```

### Methode 2 : Structures (style CMSIS)

```c
typedef struct {
    volatile uint32_t DATA;       // offset 0x000
    volatile uint32_t DIR;        // offset 0x004
    volatile uint32_t IS;         // offset 0x008
    volatile uint32_t IBE;        // offset 0x00C
    // ...
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)0x40004000)

// Utilisation
GPIOA->DIR |= (1 << 5);
GPIOA->DATA |= (1 << 5);
```

> La methode CMSIS (structures) est **recommandee** : plus lisible, auto-documentation, compatible avec l'auto-completion de l'IDE.

---

## Resume

- **Pointeurs de fonctions** : callbacks et tables de dispatch
- **Preprocesseur** : macros pour les operations sur les bits
- **volatile** : obligatoire pour registres E/S et variables ISR
- **const** : donnees en ROM | **static** : persistance/portee fichier
- **Acces registres** : pointeurs `volatile` ou structures CMSIS

---

[<< Chapitre 14](./14-Tableaux-Structures.md) | [Chapitre 16 : Processus de Compilation >>](./16-Processus-Compilation.md)
