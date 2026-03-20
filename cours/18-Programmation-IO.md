# Chapitre 18 : Programmation des Entrees/Sorties

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 17](./17-Microcontroleur.md) | [Chapitre 19 >>](./19-Serie-UART.md)

---

## Objectifs

> Programmer les peripheriques du microcontroleur : GPIO, Timers, ADC, avec des exemples pratiques.

---

## 18.1 Registres de Peripheriques

Les peripheriques sont controles via des **registres mappes en memoire** (region `0x40000000 - 0x5FFFFFFF`).

### Acces en C

```c
// Methode directe
#define GPIOA_DATA  (*(volatile uint32_t *)0x400043FC)

// Methode CMSIS (recommandee)
GPIOA->DATA |= (1 << 5);   // active bit 5
```

> **volatile** est **obligatoire** : le compilateur ne doit pas optimiser les acces aux registres E/S.

---

## 18.2 GPIO : Programmation

### Configurer une LED (sortie)

```c
// Activer l'horloge du GPIO
RCC->AHBENR |= (1 << 0);        // horloge GPIOA

// Configurer PA5 en sortie
GPIOA->DIR |= (1 << 5);

// Allumer la LED
GPIOA->DATA |= (1 << 5);

// Eteindre la LED
GPIOA->DATA &= ~(1 << 5);

// Basculer (toggle) la LED
GPIOA->DATA ^= (1 << 5);
```

### Lire un bouton (entree)

```c
// Configurer PA0 en entree avec pull-up
GPIOA->DIR &= ~(1 << 0);
GPIOA->PUR |= (1 << 0);

// Lire l'etat du bouton
if ((GPIOA->DATA & (1 << 0)) == 0) {
    // Bouton appuye (actif bas)
}
```

### Exemple : LED controlee par bouton

```c
while (1) {
    if ((GPIOA->DATA & (1 << 0)) == 0) {  // bouton appuye
        GPIOA->DATA |= (1 << 5);           // LED ON
    } else {
        GPIOA->DATA &= ~(1 << 5);          // LED OFF
    }
}
```

---

## 18.3 Timers

### Concept

Un timer est un compteur materiel qui incremente/decremente a chaque cycle d'horloge.

```
Horloge ──→ [Prescaler ÷N] ──→ [Compteur] ──→ Interruption
                                     ↑               (quand = Reload)
                                     |
                                 Rechargement
```

### Configuration

```c
// Activer l'horloge du Timer
RCC->APB1ENR |= (1 << 0);       // horloge TIM2

// Configurer le prescaler (diviser l'horloge)
TIM2->PSC = 48000 - 1;           // 48 MHz / 48000 = 1 kHz (1 ms)

// Valeur de rechargement
TIM2->ARR = 1000 - 1;            // 1000 ms = 1 seconde

// Activer l'interruption de debordement
TIM2->DIER |= (1 << 0);

// Activer le timer
TIM2->CR1 |= (1 << 0);

// Activer dans le NVIC
NVIC_EnableIRQ(TIM2_IRQn);
```

### Handler du timer

```c
void TIM2_IRQHandler(void) {
    if (TIM2->SR & (1 << 0)) {      // flag de debordement
        TIM2->SR &= ~(1 << 0);      // acquitter
        GPIOA->DATA ^= (1 << 5);    // toggle LED
    }
}
```

### PWM (Modulation de Largeur d'Impulsion)

```
Periode = ARR
Rapport cyclique = CCR / ARR

Signal :  ____++++____++++____++++
          |← Ton→|
          |←── Periode ──→|

Ton / Periode = rapport cyclique (0-100%)
```

```c
TIM2->CCR1 = 500;    // 50% duty cycle (si ARR = 999)
```

---

## 18.4 ADC (Convertisseur Analogique-Numerique)

### Principe

Convertit une tension analogique en valeur numerique :

```
Tension (0V - 3.3V)  ──→  [ADC 12 bits]  ──→  Valeur (0 - 4095)

Formule : Valeur = (Tension / Vref) × (2^resolution - 1)
Exemple : 1.65V / 3.3V × 4095 = 2047
```

### Configuration et lecture

```c
// Activer l'horloge de l'ADC
RCC->APB2ENR |= (1 << 9);

// Configurer le canal
ADC1->CHSELR = (1 << 0);          // canal 0

// Activer l'ADC
ADC1->CR |= (1 << 0);             // ADEN

// Demarrer une conversion
ADC1->CR |= (1 << 2);             // ADSTART

// Attendre la fin de conversion
while (!(ADC1->ISR & (1 << 2)));  // attendre EOC

// Lire le resultat
uint16_t value = ADC1->DR;

// Convertir en tension
float voltage = value * 3.3f / 4095.0f;
```

---

## 18.5 Anti-rebond (Debouncing)

### Le probleme

```
Signal reel du bouton :

Appui    ──┐ ┌─┐ ┌─┐ ┌───────────
           └─┘ └─┘ └─┘
           ← rebonds →  ← stable →

Le processeur detecte plusieurs transitions !
```

### Solution logicielle

```c
#define DEBOUNCE_MS 20

uint8_t read_button_debounced(void) {
    if ((GPIOA->DATA & (1 << 0)) == 0) {   // premiere lecture
        delay_ms(DEBOUNCE_MS);                // attendre
        if ((GPIOA->DATA & (1 << 0)) == 0) { // confirmer
            return 1;  // bouton reellement appuye
        }
    }
    return 0;
}
```

---

## Resume

- **Registres E/S** : mappes en memoire, acces via pointeurs `volatile`
- **GPIO** : DIR (direction), DATA (lecture/ecriture), SET/CLR/TOGGLE
- **Timers** : prescaler + compteur + rechargement, PWM pour controle moteur/LED
- **ADC** : conversion tension → valeur numerique, resolution 8-12 bits
- **Anti-rebond** : delai + double lecture pour les boutons mecaniques

---

[<< Chapitre 17](./17-Microcontroleur.md) | [Chapitre 19 : Communication Serie UART >>](./19-Serie-UART.md)
