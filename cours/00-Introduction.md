# Introduction : Types de Processeurs et ARM Cortex

[Retour a l'index](./00-INDEX.md)

---

## Objectifs de ce chapitre

> Comprendre les differents types de processeurs embarques, leurs caracteristiques materielles et logicielles, et decouvrir la famille des processeurs ARM Cortex.

---

## 1. Classification des Processeurs

Les processeurs peuvent etre classes en trois grandes categories selon leur complexite et leur domaine d'application :

```
          Complexite croissante →
+------------------+-------------------+---------------------+
| MICROCONTROLEURS | PROCESSEURS       | PROCESSEURS         |
|                  | TEMPS REEL        | D'APPLICATION       |
+------------------+-------------------+---------------------+
| Simple           | Moyen             | Complexe            |
| Basse puissance  | Haute performance | Tres haute perf.    |
| Cout faible      | Temps critique    | OS complet          |
| SoC integre      | Memoire + cache   | Multi-coeur, MMU    |
+------------------+-------------------+---------------------+
  Ex: Cortex-M       Ex: Cortex-R        Ex: Cortex-A
```

---

## 2. Processeurs Microcontroleurs

### 2.1 Materiel (Hardware)

Les microcontroleurs sont conçus pour des applications **compactes** et **economes en energie**. Ils integrent sur une **seule puce** (SoC - System on Chip) :

| Composant | Description |
|-----------|-------------|
| Processeur | Coeur a faible consommation (quelques MHz) |
| Memoire Flash | Stockage du programme (ROM) |
| SRAM | Memoire vive pour les donnees |
| Ports E/S | GPIO, Timer, UART, SPI, I2C, ADC... |
| Horloge | Oscillateur RC interne + cristal externe |
| Reset | Circuit de reinitialisation |

> **Avantages** : Taille reduite, faible consommation, faible cout, haute fiabilite

### 2.2 Logiciel (Software)

- **Systemes bare-metal** : programme simple executant une boucle infinie, sans systeme d'exploitation
- **RTOS** (Real-Time Operating System) : pour les systemes plus complexes necessitant du multitache (ex: FreeRTOS)
- **Modes d'economie d'energie** : sleep et deep sleep pour reduire la consommation

### 2.3 Applications

| Domaine | Exemples |
|---------|----------|
| Automobile | Controle ceinture, pression pneus, vitres electriques |
| Electromenager | Micro-ondes, lave-linge, refrigerateurs |
| Medical | Thermometres, oxymetre, tensiometre |
| IoT | Capteurs connectes, domotique |
| Grand public | Telecommandes, jouets, montres |

---

## 3. Processeurs Temps Reel

### 3.1 Materiel (Hardware)

Les processeurs temps reel requierent **significativement plus de puissance** de calcul :

| Caracteristique | Detail |
|----------------|--------|
| Frequence d'horloge | Plus elevee que les microcontroleurs |
| Memoire on-chip | Pour code et donnees critiques |
| Stockage externe | Carte SD, SSD pour le programme |
| RAM externe | Grande capacite pour l'execution |
| Cache | Memoire cache on-chip pour les performances |

### 3.2 Logiciel (Software)

- Utilisation d'un **RTOS** pour le multitache avec priorites
- **Bootloader** pour charger le programme depuis le stockage
- Contraintes de **temps de reponse** strictes (deadlines)

### 3.3 Applications

| Domaine | Exemples |
|---------|----------|
| Automobile | ECU (controle moteur), ABS, ESP |
| Telecom | Modem sans fil, traitement signal |
| Stockage | Controleur SSD |
| Audio | Equipement audio professionnel |
| Industriel | Automates, controle de processus |

> **Point cle** : Un systeme temps reel est defini par le fait qu'une **reponse en retard est une mauvaise reponse**. La precision peut descendre jusqu'a la nanoseconde.

---

## 4. Processeurs d'Application

### 4.1 Materiel (Hardware)

Les processeurs d'application sont les **plus puissants** :

| Caracteristique | Detail |
|----------------|--------|
| Frequence | Quelques GHz |
| Coeurs | Multi-coeur (2, 4, 8+) |
| Coprocesseurs | GPU, NPU, DSP |
| RAM | Plusieurs Go (DDR4/DDR5) |
| MMU | Unite de gestion memoire pour memoire virtuelle |
| Peripheriques | USB, Ethernet, camera, affichage |

### 4.2 Logiciel (Software)

- **Systeme d'exploitation complet** : Linux, Android, Windows
- **Virtualisation** possible (plusieurs OS simultanément)
- Composants OS : ordonnanceur, gestion memoire, systeme de fichiers, pilotes

### 4.3 Applications

| Domaine | Exemples |
|---------|----------|
| Mobile | Smartphones, tablettes |
| Informatique | PC, serveurs, cloud |
| Multimedia | Set-top box, infotainment automobile |
| Reseau | Routeurs, pare-feu |
| Medical | Imagerie medicale |
| IoT avance | Kiosques, distributeurs, IA embarquee |

---

## 5. Comparaison : Microcontroleurs vs Temps Reel

> **Quelle est la difference cle ?**

| Critere | Microcontroleur | Temps Reel |
|---------|----------------|------------|
| Puissance de calcul | Faible a moyenne | Elevee |
| Memoire | kB | MB |
| Frequence | kHz - dizaines de MHz | Centaines de MHz |
| Consommation | Tres faible | Moyenne |
| Cout | Tres faible | Moyen |
| Complexite logicielle | Simple | Moyenne a elevee |
| OS | Optionnel (bare-metal/RTOS) | RTOS typiquement |

> **Note importante** : La frontiere entre microcontroleurs et processeurs temps reel est de plus en plus floue. Les microcontroleurs modernes montent en frequence et en capacite memoire, les rendant aptes a des applications temps reel.

---

## 6. Vue d'ensemble des Processeurs ARM Cortex

### 6.1 Evolution des Architectures ARM

| Version | Caracteristiques | Processeurs |
|---------|-----------------|-------------|
| **ARMv1/v2** | Premieres versions, principes RISC | Processeurs ARM originaux |
| **ARMv3** | Licences tierces | - |
| **ARMv4/v5** | Telephones mobiles, efficacite energetique | Feature phones → smartphones |
| **ARMv6** | Introduction Cortex-A et Cortex-M | Cortex-A (haute perf), Cortex-M (basse conso) |
| **ARMv7** | Signal processing, virgule flottante, Cortex-R | Cortex-M3/M4/M7, Cortex-R |
| **ARMv8** | 64 bits (AArch64), securite, vectoriel | Cortex-A pour serveurs, mobile |
| **ARMv9** | Securite renforcee, IA/ML | Derniere generation |

### 6.2 Trois Familles ARM Cortex

```
                    ARM Cortex
                        |
          +-------------+-------------+
          |             |             |
     Cortex-A      Cortex-R      Cortex-M
     (Application)  (Real-time)   (Microcontroller)
          |             |             |
    Smartphones    Automobile    IoT, Capteurs
    Serveurs       Medical       Electromenager
    Tablettes      Industriel    Wearables
```

### 6.2.1 Cortex-A (Application)

Processeurs haute performance pour OS complets :

| Gamme | Utilisation |
|-------|-----------|
| Haut de gamme | Smartphones, tablettes, laptops |
| Milieu de gamme | Infotainment, smart home |
| Entree de gamme | Terminaux POS, panneaux industriels |

### 6.2.2 Cortex-R (Temps Reel)

Processeurs optimises pour les applications temps reel dur :

- **Pas besoin d'OS complet** - applications specifiques
- **Haute reactivite** et faible latence
- **Securite fonctionnelle** : mode lock-step, ECC memoire
- **Certifications** medicales, industrielles, automobiles

### 6.2.3 Cortex-M (Microcontroleur)

Processeurs integres dans des microcontroleurs :

| Processeur | Caracteristiques | Applications typiques |
|-----------|-----------------|----------------------|
| **Cortex-M0** | Le plus petit, entree de gamme | Microcontroleurs basiques |
| **Cortex-M0+** | Ultra basse consommation | Wearables, sante, fitness |
| **Cortex-M3** | Jeu d'instructions plus puissant | Maison intelligente |
| **Cortex-M4** | DSP + FPU optionnelle simple precision | Signal mixte, controle |
| **Cortex-M7** | Haute performance, cache, memoire externe | Automatisation intensive |
| **Cortex-M23** | TrustZone securite | Montres, securite maison |
| **Cortex-M33** | TrustZone securite | Securite avancee |
| **Cortex-M35P** | Protection physique | Donnees confidentielles |
| **Cortex-M52** | Helium (vectoriel) | Traitement signal |
| **Cortex-M55** | Helium (vectoriel) | IA embarquee, audio |
| **Cortex-M85** | Helium (vectoriel) | Reconnaissance vocale, image |

---

## 7. Le Cortex-M0 en Detail

Le Cortex-M0 est le processeur etudie dans ce livre. C'est le **plus petit et le plus simple** de la famille Cortex-M.

### Caracteristiques principales

| Parametre | Valeur |
|-----------|--------|
| Architecture | ARMv6-M |
| Taille du mot | 32 bits |
| Jeu d'instructions | Thumb (sous-ensemble compact) |
| Pipeline | 3 etages (Fetch, Decode, Execute) |
| Nombre d'instructions | 56 instructions uniques |
| Espace d'adressage | 4 Go (32 bits) |
| Interruptions | Jusqu'a 32 (via NVIC) |
| Niveaux de priorite | 4 (2 bits) |
| Modes d'alimentation | Sleep, Deep sleep |

### Diagramme bloc

```
  Interface gestion energie
         |
+--------|---------------------------------------+
|  +-----v-----+                                 |
|  |    WIC    |     Processeur Cortex-M0         |
|  | (optionnel)|                                 |
|  +-----+-----+                                 |
|        |                                        |
|  +-----v-----+    +------------------+          |
|  |   NVIC    |--->| Coeur Cortex-M0  |          |
|  +-----------+    +--------+---------+          |
|                            |                    |
|  Interruptions        Bus interne               |
|                            |                    |
|                   +--------v---------+          |
|                   |  Unite Debogage  |<-------> Debugger
|                   |   (optionnelle)  |          |
|                   +------------------+          |
+------------------------|------------------------+
                         |
                Interface AHB-Lite
                         |
          +--------------+--------------+
          |              |              |
     +----+----+   +----+----+   +-----+-----+
     |  Flash  |   |  SRAM   |   | Periph.   |
     | (Code)  |   | (Data)  |   | (E/S)     |
     +---------+   +---------+   +-----------+
```

### Composants du processeur

| Composant | Role |
|-----------|------|
| **Coeur Cortex-M0** | Le "cerveau" - execute les instructions |
| **NVIC** | Gere les interruptions des peripheriques |
| **SysTick** | Timer systeme 24 bits (reference temporelle) |
| **WIC** | Reveil depuis le mode sleep (optionnel) |
| **Debug** | Chargement du programme, controle d'execution |

---

## 8. Notes Importantes

> **Coeurs ARM vs Processeurs**
>
> Les coeurs ARM sont des **designs de processeurs** que les fabricants de semi-conducteurs licensient et integrent dans leurs puces. On ne peut **pas acheter un processeur ARM seul** - il faut acheter une puce complete d'un fabricant (ST, NXP, TI, etc.) qui contient un coeur ARM.

> **Terminologie**
>
> - **Coeur** (*core*) : le design du microprocesseur (ex: Cortex-M0 core)
> - **Processeur** (*processor*) : la puce integree contenant le coeur + peripheriques
> - En parlant d'architecture, "processeur" = coeur
> - En parlant de produit, "processeur" = puce integree

---

## Resume

```
+-----------------------------------------------------------+
| POINTS CLES                                                |
+-----------------------------------------------------------+
| 1. Trois types de processeurs :                            |
|    - Microcontroleur (simple, basse conso, SoC)            |
|    - Temps reel (performance, deadlines strictes)          |
|    - Application (haute perf, OS complet, multi-coeur)     |
|                                                            |
| 2. ARM Cortex definit trois familles :                     |
|    - Cortex-A : processeurs d'application                  |
|    - Cortex-R : processeurs temps reel                     |
|    - Cortex-M : microcontroleurs                           |
|                                                            |
| 3. Cortex-M0 :                                             |
|    - Architecture ARMv6-M, 32 bits                         |
|    - Plus petit processeur de la famille Cortex-M          |
|    - 56 instructions (jeu Thumb compact)                   |
|    - Pipeline 3 etages                                     |
|    - NVIC integre pour les interruptions                   |
|    - Ideal pour applications basse consommation            |
+-----------------------------------------------------------+
```

---

[Chapitre suivant : Systeme Microprocesseur >>](./01-Systeme-Microprocesseur.md)
