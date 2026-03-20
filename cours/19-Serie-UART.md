# Chapitre 19 : Communication Serie - UART

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 18](./18-Programmation-IO.md) | [Annexe A >>](./A-Keil-uVision.md)

---

## Objectifs

> Comprendre et programmer la communication serie UART sur le Cortex-M0.

---

## 19.1 Communication Serie vs Parallele

| Critere | Parallele | Serie |
|---------|-----------|-------|
| Fils | N fils de donnees | 1-2 fils de donnees |
| Vitesse | Rapide (courte distance) | Variable |
| Distance | Courte | Longue |
| Cout | Eleve (nombreux fils) | Faible |
| Exemples | Bus memoire | UART, SPI, I2C, USB |

### Types de communication serie

| Type | Horloge | Fils | Exemple |
|------|---------|------|---------|
| **Synchrone** | Signal d'horloge partage | CLK + DATA | SPI, I2C |
| **Asynchrone** | Pas d'horloge partagee | TX + RX | **UART** |

---

## 19.2 Protocole UART

### Format de trame

```
Repos    Start  D0  D1  D2  D3  D4  D5  D6  D7  [P]  Stop   Repos
(haut)    (bas) ←────── 8 bits de donnees ──────→ [Parite] (haut) (haut)
──────┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──────
      └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘
```

| Element | Description |
|---------|-------------|
| **Bit de start** | 1 bit, toujours 0 (bas) |
| **Donnees** | 5 a 9 bits (typiquement 8) |
| **Parite** | Optionnel (pair, impair, ou aucune) |
| **Bit(s) de stop** | 1 ou 2 bits, toujours 1 (haut) |

### Baud Rate (Vitesse)

| Baud Rate | Bits/seconde | Utilisation |
|-----------|-------------|-------------|
| 9600 | 9 600 | Debug, basse vitesse |
| 19200 | 19 200 | Communication standard |
| 38400 | 38 400 | Communication standard |
| 115200 | 115 200 | **Le plus courant** |
| 921600 | 921 600 | Haute vitesse |

> Les deux extremites (emetteur et recepteur) **doivent** utiliser le **meme** baud rate.

### Full-Duplex

```
   MCU                      PC
+--------+              +--------+
|     TX |──────────────| RX     |
|     RX |──────────────| TX     |
|    GND |──────────────| GND    |
+--------+              +--------+

TX et RX sont independants → communication simultanee dans les deux sens
```

---

## 19.3 Registres UART

| Registre | Nom | Fonction |
|----------|-----|----------|
| **DR** | Data Register | Donnee a envoyer / donnee reçue |
| **SR** | Status Register | Drapeaux d'etat |
| **BRR** | Baud Rate Register | Configuration vitesse |
| **CR1** | Control Register 1 | Enable TX/RX, interruptions |

### Drapeaux d'etat (SR)

| Drapeau | Signification |
|---------|---------------|
| **TXE** (TX Empty) | 1 = registre TX vide, pret a envoyer |
| **RXNE** (RX Not Empty) | 1 = donnee reçue disponible |
| **TC** (Transmission Complete) | 1 = transmission terminee |
| **ORE** (Overrun Error) | 1 = donnee perdue (pas lue a temps) |
| **FE** (Framing Error) | 1 = erreur de trame |

---

## 19.4 Configuration UART

```c
// 1. Activer les horloges
RCC->APB2ENR |= (1 << 14);     // horloge USART1
RCC->AHBENR  |= (1 << 0);      // horloge GPIOA

// 2. Configurer les broches GPIO en mode alternatif
GPIOA->AFSEL |= (1 << 9) | (1 << 10);  // PA9=TX, PA10=RX
// Configurer la fonction alternative UART

// 3. Configurer le baud rate
// BRR = horloge_peripherique / baud_rate
// Exemple : 48 MHz / 115200 = 416.67 → BRR = 417
USART1->BRR = 417;

// 4. Activer TX et RX
USART1->CR1 = (1 << 3) |       // TE : TX enable
               (1 << 2) |       // RE : RX enable
               (1 << 0);        // UE : USART enable
```

---

## 19.5 Emission et Reception

### Envoyer un caractere (polling)

```c
void uart_send_char(char c) {
    while (!(USART1->SR & (1 << 7)));  // attendre TXE = 1
    USART1->DR = c;                      // envoyer le caractere
}
```

### Recevoir un caractere (polling)

```c
char uart_recv_char(void) {
    while (!(USART1->SR & (1 << 5)));  // attendre RXNE = 1
    return (char)(USART1->DR);           // lire le caractere
}
```

### Envoyer une chaine

```c
void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

// Utilisation
uart_send_string("Hello, World!\r\n");
```

### Echo (recevoir et renvoyer)

```c
while (1) {
    char c = uart_recv_char();   // recevoir
    uart_send_char(c);            // renvoyer (echo)
}
```

### Reception par interruption

```c
// Activer l'interruption RXNE
USART1->CR1 |= (1 << 5);        // RXNEIE
NVIC_EnableIRQ(USART1_IRQn);

// Buffer de reception
volatile char rx_buffer[64];
volatile uint8_t rx_index = 0;

void USART1_IRQHandler(void) {
    if (USART1->SR & (1 << 5)) {           // RXNE
        rx_buffer[rx_index++] = USART1->DR;  // lire (acquitte auto)
        if (rx_index >= 64) rx_index = 0;    // protection debordement
    }
}
```

---

## 19.6 Communication avec un PC

### Materiel necessaire

```
MCU (UART 3.3V)          Convertisseur         PC
+---------+          +---------------+      +--------+
| TX (3.3V)|─────────| RX    USB TX  |──────| USB    |
| RX (3.3V)|─────────| TX    USB RX  |      |        |
| GND      |─────────| GND   USB GND |      |        |
+---------+          +---------------+      +--------+
                     (CP2102, CH340, FTDI)
```

### Logiciel terminal

| Logiciel | Plateforme |
|----------|-----------|
| PuTTY | Windows/Linux |
| Tera Term | Windows |
| minicom | Linux |
| screen | Linux/Mac |

### Configuration du terminal

| Parametre | Valeur typique |
|-----------|---------------|
| Baud rate | 115200 |
| Data bits | 8 |
| Stop bits | 1 |
| Parite | None |
| Flow control | None |

---

## Resume

- **UART** : communication serie asynchrone, full-duplex (TX + RX)
- **Trame** : Start + 8 bits donnees + [Parite] + Stop
- **Baud rate** : 115200 le plus courant, BRR = horloge / baud_rate
- **Emission** : attendre TXE, ecrire dans DR
- **Reception** : attendre RXNE, lire DR (polling ou interruption)
- **PC** : convertisseur USB-UART + logiciel terminal

---

[<< Chapitre 18](./18-Programmation-IO.md) | [Annexe A : Keil uVision >>](./A-Keil-uVision.md)
