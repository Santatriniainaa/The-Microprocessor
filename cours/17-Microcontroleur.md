# Chapitre 17 : Le Microcontroleur

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 16](./16-Processus-Compilation.md) | [Chapitre 18 >>](./18-Programmation-IO.md)

---

## Objectifs

> Comprendre l'architecture interne d'un microcontroleur : systeme d'horloge, reset, gestion d'energie, et GPIO.

---

## 17.1 Architecture d'un Microcontroleur (SoC)

```
+----------------------------------------------------------------+
|                      Microcontroleur (SoC)                      |
|                                                                  |
|  +------------------+                                            |
|  | Cortex-M0 Core   |                                            |
|  | +------+ +-----+ |                                            |
|  | | NVIC | | DBG | |                                            |
|  | +------+ +-----+ |                                            |
|  +--------+---------+                                            |
|           |                                                      |
|    Bus AHB (haute performance)                                   |
|    ───────┬──────────┬──────────────                             |
|           |          |                                            |
|    +------+---+ +----+-----+                                     |
|    |  Flash   | |   SRAM   |                                     |
|    | (ROM)    | |   (RAM)  |                                     |
|    +----------+ +----------+                                     |
|                                                                  |
|    Bus APB (peripheriques)                                       |
|    ───────┬────────┬────────┬────────┬──────────                 |
|           |        |        |        |                            |
|    +------++ +-----++ +-----++ +-----++                          |
|    | GPIO  | | Timer| | UART | | ADC  |                          |
|    +-------+ +------+ +------+ +------+                          |
|                                                                  |
|  +------------------+  +-------------------+                     |
|  | Systeme Horloge  |  | Reset + Gestion   |                    |
|  | (RC, XTAL, PLL)  |  | Energie           |                    |
|  +------------------+  +-------------------+                     |
+----------------------------------------------------------------+
```

---

## 17.2 Systeme d'Horloge

### Sources d'horloge

| Source | Precision | Frequence | Utilisation |
|--------|----------|-----------|-------------|
| **RC interne** | ±1-5% | 4-16 MHz | Demarrage rapide, economique |
| **Cristal externe** | ±20 ppm | 4-25 MHz | Haute precision |
| **PLL** | Haute (multipliee) | Jusqu'a 200+ MHz | Frequence CPU elevee |

### Arbre d'horloge (Clock Tree)

```
Cristal (12 MHz)
    |
    v
+-------+      +--------+
|  PLL  |─────→| ÷1..÷N |──→ Horloge CPU (48 MHz)
| ×N    |      +--------+
+-------+          |
                   +──→ ÷2 ──→ Horloge APB (24 MHz) ──→ Peripheriques
                   |
                   +──→ ÷1 ──→ Horloge AHB (48 MHz) ──→ Memoire, DMA
```

### Clock Gating

Desactiver l'horloge des peripheriques non utilises pour **economiser l'energie** :

```c
// Activer l'horloge du GPIO port A
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

// Desactiver l'horloge du Timer 2
RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
```

---

## 17.3 Systeme de Reset

| Type de reset | Declencheur |
|--------------|-------------|
| **Power-On Reset (POR)** | Mise sous tension |
| **Reset externe** | Bouton RESET |
| **Watchdog Reset** | Timer watchdog expire (systeme bloque) |
| **Software Reset** | Instruction `NVIC_SystemReset()` |

### Sequence de reset

```
1. Tous les registres → valeurs par defaut
2. Toutes les broches E/S → etat haute impedance (entree)
3. SP ← Mem[0x00000000]
4. PC ← Mem[0x00000004] (Reset_Handler)
5. Execution du code de demarrage
```

---

## 17.4 Gestion de l'Energie

| Mode | CPU | Peripheriques | Memoire | Reveil |
|------|-----|--------------|---------|--------|
| **Actif** | Marche | Marche | Marche | - |
| **Sleep** | Arrete | Marche | Marche | Interruption (rapide) |
| **Deep Sleep** | Arrete | La plupart arretes | Marche | Quelques sources (lent) |

### Instructions de mise en veille

```asm
WFI     // Wait For Interrupt : dort jusqu'a la prochaine IRQ
WFE     // Wait For Event : dort jusqu'au prochain evenement
```

```c
// En C (CMSIS)
__WFI();    // entrer en mode sleep
__WFE();    // entrer en mode sleep (evenement)
```

---

## 17.5 GPIO (General Purpose Input/Output)

### Registres GPIO typiques

| Registre | Fonction |
|----------|----------|
| **DIR** (Direction) | 0 = entree, 1 = sortie |
| **DATA** | Lire l'etat des entrees / ecrire les sorties |
| **SET** | Mettre des bits a 1 (sans affecter les autres) |
| **CLR** | Mettre des bits a 0 (sans affecter les autres) |
| **TOGGLE** | Inverser des bits |
| **PUR** | Pull-up enable |
| **PDR** | Pull-down enable |
| **AFSEL** | Fonction alternative (UART, SPI, etc.) |

### Configuration d'une broche

```c
// Configurer PA5 en sortie (LED)
GPIOA->DIR |= (1 << 5);      // direction = sortie

// Configurer PA0 en entree (bouton)
GPIOA->DIR &= ~(1 << 0);     // direction = entree
GPIOA->PUR |= (1 << 0);      // activer pull-up
```

```
Broche GPIO :

                 VCC
                  |
             [Pull-up R]     ← active par PUR
                  |
MCU Pin ──────────+──────── Signal externe
                  |
             [Pull-down R]   ← active par PDR
                  |
                 GND
```

---

## Resume

- **SoC** : Cortex-M0 + Flash + SRAM + Peripheriques sur une puce
- **Horloge** : RC/cristal → PLL → arbre d'horloge → clock gating
- **Reset** : POR, externe, watchdog, software → SP + PC initialises
- **Energie** : Sleep (WFI), Deep Sleep pour economiser
- **GPIO** : registres DIR/DATA/SET/CLR, pull-up/pull-down

---

[<< Chapitre 16](./16-Processus-Compilation.md) | [Chapitre 18 : Programmation des E/S >>](./18-Programmation-IO.md)
