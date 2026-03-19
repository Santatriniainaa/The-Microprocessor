# Chapitre 1 : Systeme Microprocesseur

[Retour a l'index](./00-INDEX.md) | [Chapitre suivant >>](./02-Architecture-Jeu-Instructions.md)

---

## Objectifs

> Dans ce chapitre, nous allons nous familiariser avec les bases de l'organisation materielle d'un systeme microprocesseur et comprendre comment les programmes s'executent dans un tel systeme.

**Questions cles :**
- Quels sont les composants d'un systeme microprocesseur ?
- Comment les donnees sont-elles stockees et accedees en memoire ?
- Quels sont les differents types de memoire ?
- Comment fonctionnent les bus ?
- Comment les ports E/S interagissent-ils avec le processeur ?
- Qu'est-ce qu'une interruption ?

---

## 1.1 Introduction

Un systeme microprocesseur simple se compose de :

```
+-------------------------------------------------------------------+
|                    Systeme Microprocesseur                          |
|                                                                    |
|  +-------+          +-------------------+                          |
|  |       |   Bus    |     Memoire       |                          |
|  |  CPU  |<========>|  +------+------+  |                          |
|  |       |          |  | ROM  | RAM  |  |                          |
|  +---+---+          |  +------+------+  |                          |
|      |              +-------------------+                          |
|      |                                                             |
|      | Bus          +-------------------+     +-----------+        |
|      +=============>|    Ports E/S      |<--->| Dispositif|        |
|      |              +-------------------+     |   E/S     |        |
|      |                      |                 +-----------+        |
|      |              Interruptions (IRQ)                             |
|      |                                                             |
|  +---+----------------------------------------------+              |
|  |      Horloge, Reset et Gestion d'Energie         |              |
|  +--------------------------------------------------+              |
+-------------------------------------------------------------------+
```

| Composant | Role |
|-----------|------|
| **CPU** (Unite Centrale) | Execute les instructions du programme |
| **Memoire** | Stocke le programme (instructions + donnees) |
| **Ports E/S** | Interface entre le CPU et les peripheriques externes |
| **Bus** | Lignes electriques reliant CPU, memoire et ports E/S |
| **Reset** | Initialise le systeme au demarrage |
| **Horloge** | Synchronise tous les composants |
| **Gestion d'energie** | Alimente et gere les modes basse consommation |

> **Le CPU n'a pas d'intelligence propre.** Il execute simplement les instructions du programme stocke en memoire. C'est le **programme** qui determine la fonctionnalite du systeme.

---

## 1.2 Processeur

Un microprocesseur est un **circuit integre** compose de milliers a milliards de transistors.

### Facteurs determinant les capacites d'un processeur

#### 1.2.1 Taille du mot (Word Size)

Le nombre de bits qu'un processeur peut traiter en une seule instruction.

| Taille du mot | Utilisation |
|--------------|-------------|
| 8 bits | Microcontroleurs tres simples |
| 16 bits | Microcontroleurs intermediaires |
| **32 bits** | **Cortex-M0** (et la plupart des MCU modernes) |
| 64 bits | Processeurs d'application puissants |

> **Definition precise** : La taille du mot est la largeur des registres a usage general. Par exemple, le Cortex-M0 a des registres de 32 bits → architecture 32 bits.

#### 1.2.2 Vitesse d'horloge (Clock Speed)

| Plage | Utilisation |
|-------|-----------|
| kHz | Applications ultra basse consommation |
| MHz | Microcontroleurs typiques |
| GHz | Processeurs d'application |

#### 1.2.3 Jeu d'instructions (Instruction Set)

- **CISC** (Complex Instruction Set) : instructions complexes, tailles variables
- **RISC** (Reduced Instruction Set) : instructions simples, taille fixe, pipeline efficace

> **ARM utilise l'architecture RISC.** Le jeu d'instructions comprend les instructions **ARM** (32 bits) et **Thumb** (16 bits). Le Cortex-M0 utilise un sous-ensemble compact de **Thumb**.

#### 1.2.4 Nombre de coeurs

Les processeurs multi-coeurs executent des instructions en parallele sur plusieurs coeurs independants.

#### Accelerateurs materiels

| Accelerateur | Fonction |
|-------------|----------|
| **FPU** | Operations en virgule flottante |
| **Crypto** | Chiffrement/dechiffrement temps reel |
| **GPU** | Traitement graphique |
| **DSP** | Traitement numerique du signal |
| **NPU** | Reseaux neuronaux / IA |

---

## 1.3 Memoire

### Unites de base

| Unite | Taille | Nom ARM (Cortex-M) | Type C |
|-------|--------|--------------------|----|
| **Bit** | 1 bit | - | - |
| **Octet** (byte) | 8 bits | byte | `char` |
| **Demi-mot** (half-word) | 16 bits | short | `short` |
| **Mot** (word) | 32 bits | word | `int` / `long` |

> **Approximations utiles** : 1 kB = 2^10 = 1 024 octets ≈ 10^3, 1 MB = 2^20 ≈ 10^6, 1 GB = 2^30 ≈ 10^9

### 1.3.1 Contenu de la memoire

Les memes bits en memoire peuvent representer des choses differentes selon le contexte :

```
Memoire a l'adresse X :  0 1 0 0 0 0 0 1

Interprete comme :
  - Binaire :     0b01000001
  - Hexadecimal : 0x41
  - Decimal :     65
  - Caractere :   'A' (ASCII)
```

**Exemple d'instruction en memoire :**

```
2 octets en memoire : 0100 0100  0011 0101
                       └── Code instruction ──┘
En binaire :  0b0100010000110101
En hex :      0x4435
En decimal :  17461
En instruction ARM : ADD R5, R6
```

### 1.3.2 Adresse

- Chaque **octet** en memoire a une **adresse unique**
- Le Cortex-M0 a **32 lignes d'adresse** → 2^32 = **4 Go** d'espace adressable
- Plage : `0x00000000` a `0xFFFFFFFF`

```
L'adresse d'un mot ou demi-mot = adresse de son premier octet

Exemple : Mot a l'adresse 0x100
  → Octets aux adresses 0x100, 0x101, 0x102, 0x103
```

### 1.3.3 Mappage memoire (Memory Mapping)

Chaque dispositif memoire est **mappe** a une plage d'adresses specifique :

```
0x00000000 ┌─────────────┐
           │             │
           │  ROM 128 kB │  Adresses : 0x00000000 - 0x0001FFFF
           │             │
0x0001FFFF └─────────────┘
           ...  (vide)  ...
0x20000000 ┌─────────────┐
           │  RAM 4 kB   │  Adresses : 0x20000000 - 0x20000FFF
0x20000FFF └─────────────┘
```

> **Formule** : Un dispositif de N octets avec adresse de debut SA est mappe aux adresses **SA** a **SA + N - 1**

---

## 1.4 Dispositifs Memoire

### Classification

```
                    Memoire
                      |
            +---------+---------+
            |                   |
          ROM                 RAM
      (Non-volatile)       (Volatile)
            |                   |
      +-----+-----+      +-----+-----+
      |           |      |           |
   OTP ROM     Flash   SRAM       DRAM
              |                     |
         +----+----+          +-----+-----+
         |         |          |     |     |
       NOR      NAND       DDR2  DDR3  DDR4
```

### 1.4.1 Dispositifs ROM

| Type | Caracteristiques | Utilisation |
|------|-----------------|-------------|
| **OTP ROM** | Programmable une seule fois, immuable | ID produit, cles de securite |
| **Flash NOR** | Reprogrammable, fiable, cout eleve | Programme du MCU (primaire) |
| **Flash NAND** | Reprogrammable, grande capacite, moins fiable | Stockage (SD, SSD) |

### 1.4.2 Dispositifs RAM

| Type | Caracteristiques | Utilisation |
|------|-----------------|-------------|
| **SRAM** | Rapide, chere, flip-flops, pas de rafraichissement | Memoire on-chip du MCU |
| **DRAM** | Moins rapide, condensateurs, rafraichissement necessaire | RAM externe (serveurs, PC) |
| **DDR SDRAM** | Transfert sur fronts montant et descendant | DDR2, DDR3, DDR4, DDR5 |

### 1.4.3 Memoire On-chip vs Off-chip

| Critere | On-chip | Off-chip |
|---------|---------|----------|
| Vitesse | Meme vitesse que le CPU | Plus lente |
| Taille | Limitee (kB a quelques MB) | Grande (MB a GB) |
| Cout | Eleve (silicium) | Moins cher par bit |
| Fiabilite | Elevee | PCB + connexion |

> **Cache** : Memoire rapide on-chip qui copie automatiquement les donnees frequemment accedees depuis la memoire externe. L1 (petite, rapide) et L2 (plus grande, moins rapide).

### 1.4.4 Memoire primaire vs secondaire

| Type | Connexion | Exemples |
|------|-----------|----------|
| **Primaire** | Via le bus (acces direct CPU) | Flash, SRAM on-chip |
| **Secondaire** | Via un port E/S (systeme de fichiers) | Carte SD, SSD |

> Le Cortex-M0 utilise typiquement **Flash + SRAM on-chip** comme memoire primaire.

---

## 1.5 Bus

Un bus est un ensemble de lignes electriques transportant des signaux numeriques.

```
+-------+   +-------+   +-------+   +-------+
|  CPU  |   |  ROM  |   |  RAM  |   | Autres|
+---+---+   +---+---+   +---+---+   +---+---+
    |           |           |           |
====|===========|===========|===========|====  ← Horloge
====|===========|===========|===========|====  ← Bus d'adresses
====|===========|===========|===========|====  ← Bus de donnees
====|===========|===========|===========|====  ← Bus de controle
```

### Composants du bus

| Bus | Largeur (Cortex-M0) | Direction | Fonction |
|-----|---------------------|-----------|----------|
| **Bus d'adresses** | 32 bits | CPU → Dispositifs | Adresse du dispositif a acceder |
| **Bus de donnees** | 32 bits | Bidirectionnel | Donnees transferees |
| **Bus de controle** | Variable | Bidirectionnel | Type et etat du transfert |

### Signaux de controle

| Signal | Valeurs | Description |
|--------|---------|-------------|
| `READ/WRITE` | 0 = lecture, 1 = ecriture | Direction du transfert |
| `SIZE` | 000=8bits, 001=16bits, 010=32bits | Taille des donnees |
| `ACK` | 1 = succes, 0 = en cours | Acquittement de l'operation |
| `ERROR` | 1 = erreur, 0 = OK | Faute de bus |

### 1.5.1 Operation du bus

**Exemple : Operation de LECTURE**

> Le processeur lit une instruction 16 bits a l'adresse `0x00000200`

```
1. CPU → Bus d'adresses : 0x00000200
2. CPU → SIZE : 001 (16 bits)
3. CPU → READ/WRITE : 0 (lecture)
4. ROM activee (adresse dans sa plage)
5. ROM → Bus de donnees : 0x4435 (instruction)
6. ROM → ACK : 1 (succes)
7. CPU lit la valeur du bus de donnees
```

**Exemple : Operation d'ECRITURE**

> Le CPU ecrit `0x12345678` a l'adresse RAM `0x20002000`

```
1. CPU → Bus d'adresses : 0x20002000
2. CPU → Bus de donnees : 0x12345678
3. CPU → SIZE : 010 (32 bits)
4. CPU → READ/WRITE : 1 (ecriture)
5. RAM activee (adresse dans sa plage)
6. RAM ecrit les 4 octets
7. RAM → ACK : 1 (succes)
```

### 1.5.2 Faute de bus (Bus Fault)

Si le CPU tente d'acceder a une adresse **non mappee** (ex: `0x10000000` sans dispositif), le bus repond avec `ERROR = 1`. Le CPU traite cela comme une **faute de bus** (HardFault).

### Protocoles AMBA

| Protocole | Utilisation | Vitesse |
|-----------|-----------|---------|
| **APB** (Advanced Peripheral Bus) | Peripheriques lents (GPIO, UART) | Basse |
| **AHB** (Advanced High-performance Bus) | Memoire, peripheriques rapides | Haute |

> Le Cortex-M0 utilise **AHB-Lite** pour la memoire et **APB** pour les peripheriques lents.

---

## 1.6 Ports E/S

Le CPU communique avec les dispositifs externes **uniquement via les bus**. Les ports E/S servent de **pont** entre le bus et le monde reel.

### Structure d'un port E/S

```
                    Port E/S
+------+     +---+------------------+---+     +-----------+
|      |     |   | Registre E/S     |   |     |           |
|      |     |   | Registre E/S     |   |     | Dispositif|
| CPU  |<===>|   | Registre E/S     |   |<--->|  Externe  |
|      | Bus |   |    ...           |   | Signaux         |
|      |     |   +------------------+   | analogiques/    |
+------+     |   | Circuit         |   | numeriques      |
             |   | d'interface     |   |     +-----------+
             +---+------------------+---+
```

### Registres E/S

- **Mappes en memoire** : chaque registre a une adresse
- Le CPU lit/ecrit les registres comme des emplacements memoire
- La **difference** : ecrire dans un registre E/S peut declencher une action sur le dispositif externe

### Exemples de ports E/S

| Port | Fonction |
|------|----------|
| **GPIO** | Entrees/sorties numeriques (LED, bouton) |
| **Timer** | Reference temporelle, generation PWM |
| **UART** | Communication serie |
| **SPI / I2C** | Communication avec capteurs, memoire |
| **ADC** | Conversion analogique → numerique |
| **SDIO** | Interface carte SD |
| **USB** | Bus serie universel |

### Interruptions

Quand un dispositif a besoin de l'attention du processeur, il envoie un signal **IRQ** (Interrupt Request).

```
+------+         Bus          +----------+
|      |<===================>|          |
| CPU  |                      | Port E/S |
|      |<--------- IRQ ------|          |
+------+                      +----------+
```

1. Le port E/S envoie un signal **IRQ** au CPU
2. Le CPU **interrompt** son activite en cours
3. Le CPU execute la **routine de service d'interruption** (ISR)
4. Une fois l'ISR terminee, le CPU **reprend** son activite

> **NVIC** (Nested Vectored Interrupt Controller) : Controleur d'interruptions integre au Cortex-M0. Il reçoit les IRQ de tous les ports et les transmet au CPU selon leur **priorite**.

---

## 1.7 Reset, Horloge et Gestion d'Energie

### 1.7.1 Reset

Le circuit de reset initialise **tous les blocs** du systeme a un etat connu au demarrage.

> **Pourquoi c'est important ?** Sans reset, les broches E/S pourraient etre dans des etats aleatoires au demarrage, allumant accidentellement un moteur ou un element chauffant !

### 1.7.2 Horloge

Tous les composants doivent fonctionner de maniere **synchrone**.

| Source | Precision | Frequence |
|--------|----------|-----------|
| Oscillateur RC interne | Moyenne | Quelques MHz |
| Cristal externe | Haute | Dizaines de MHz |
| PLL | Haute (multipliee) | Centaines de MHz |

> **PLL** (Phase-Locked Loop) : Multiplie la frequence. Exemple : cristal 12.5 MHz × 16 = CPU a 200 MHz.

### 1.7.3 Gestion d'energie

| Mode | Description | Reveil |
|------|-------------|--------|
| **Actif** | CPU et tous peripheriques en marche | - |
| **Sleep** | CPU arrete, memoire et E/S alimentes | Rapide (interruption) |
| **Deep Sleep** | La plupart des horloges arretees | Plus lent |

---

## 1.8 Vue d'ensemble du ARM Cortex-M0

### Architecture ARMv6-M

Le Cortex-M0 est base sur l'architecture **ARMv6-M** :
- Processeur **32 bits**
- Espace d'adressage total : **4 Go**
- Pipeline **3 etages**
- Jeu d'instructions **Thumb** compact

### Carte memoire standardisee

La carte memoire est divisee en **8 regions** de 512 Mo chacune :

```
0x00000000 ┌──────────────────┐
           │     Code         │  Flash/ROM (programme + constantes)
0x1FFFFFFF ├──────────────────┤
0x20000000 │     SRAM         │  Variables, pile, tas
0x3FFFFFFF ├──────────────────┤
0x40000000 │   Peripherique   │  Registres des ports E/S
0x5FFFFFFF ├──────────────────┤
0x60000000 │   RAM externe    │  Memoire externe (si presente)
0x7FFFFFFF ├──────────────────┤
0x80000000 │   RAM externe    │  Memoire externe (si presente)
0x9FFFFFFF ├──────────────────┤
0xA0000000 │  Device externe  │  Peripheriques externes
0xBFFFFFFF ├──────────────────┤
0xC0000000 │  Device externe  │  Peripheriques externes
0xDFFFFFFF ├──────────────────┤
0xE0000000 │    Systeme       │  NVIC, SysTick, Debug
0xFFFFFFFF └──────────────────┘
```

| Region | Plage | Utilisation typique |
|--------|-------|-------------------|
| **Code** | `0x00000000 - 0x1FFFFFFF` | Flash et ROM |
| **SRAM** | `0x20000000 - 0x3FFFFFFF` | Variables et pile |
| **Peripherique** | `0x40000000 - 0x5FFFFFFF` | Registres E/S |
| **Systeme** | `0xE0000000 - 0xFFFFFFFF` | NVIC, SysTick, Debug |

> Les 3 regions essentielles a retenir : **Code**, **SRAM** et **Peripherique**. La region **Systeme** est utilisee pour les peripheriques du processeur (NVIC, SysTick).

---

## 1.9 Resume

### Systeme microprocesseur
- Un systeme simple = microprocesseur + memoire + ports E/S + reset/horloge/energie

### Processeur
- Capacite determinee par : taille du mot, vitesse d'horloge, jeu d'instructions, nombre de coeurs

### Memoire
- Cellule = 1 bit, octet = 8 bits, demi-mot = 16 bits, mot = 32 bits
- Chaque octet a une adresse unique
- Espace d'adressage 32 bits = 4 Go (`0x00000000` - `0xFFFFFFFF`)
- Mappage : chaque dispositif mappe a une plage [SA, SA+N-1]

### Dispositifs memoire
- **ROM** : non-volatile, lecture seule (OTP, Flash NOR/NAND)
- **RAM** : volatile, lecture/ecriture (SRAM, DRAM)
- **On-chip** : rapide, petite | **Off-chip** : plus lente, grande

### Bus
- Bus d'adresses + bus de donnees + bus de controle
- Signaux : READ/WRITE, SIZE, ACK, ERROR
- Faute de bus si adresse non mappee

### Ports E/S
- Pont entre CPU et monde exterieur via registres mappes en memoire
- IRQ pour signaler des evenements au CPU
- NVIC gere les interruptions sur Cortex-M0

### Cortex-M0
- Architecture ARMv6-M, 32 bits
- Carte memoire standardisee en 8 regions de 512 Mo
- Composants : Coeur, NVIC, SysTick, WIC (optionnel), Debug

---

[<< Introduction](./00-Introduction.md) | [Chapitre 2 : Architecture du Jeu d'Instructions >>](./02-Architecture-Jeu-Instructions.md)
