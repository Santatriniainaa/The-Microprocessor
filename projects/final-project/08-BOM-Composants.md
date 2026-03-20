# 08 - BOM & Liste Complete des Composants

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0

---

## 1. Composants Electroniques (Hardware)

### 1.1 Composants Actifs

| # | Composant | Reference | Package | Qte | Prix unit. | Total | Fournisseur | Role |
|---|-----------|-----------|---------|-----|-----------|-------|-------------|------|
| 1 | MCU ARM Cortex-M0 | **STM32F030F4P6** | TSSOP-20 | 1 | $0.80 | $0.80 | LCSC / Mouser | Coeur du systeme |
| 2 | Capteur courant/tension I2C | **INA219AIDR** | SOIC-8 | 2 | $1.00 | $2.00 | Mouser / Digikey | Mesure I_panel + I_load |
| 3 | MOSFET N-Channel 55V 17A | **IRF540NPBF** | TO-220 | 4 | $0.30 | $1.20 | LCSC | Q0=MPPT, Q1-Q3=charges |
| 4 | Driver MOSFET dual | **TC4427ACPA** | DIP-8 | 2 | $0.50 | $1.00 | Mouser | Commande gate MOSFET |
| 5 | Regulateur LDO 3.3V 1A | **AMS1117-3.3** | SOT-223 | 1 | $0.15 | $0.15 | LCSC | Alimentation MCU |
| 6 | Diode Schottky 5A 40V | **SS54** | DO-214AC | 2 | $0.15 | $0.30 | LCSC | Protection anti-retour |
| 7 | Diode TVS 30V bidir. | **SMBJ30CA** | SMB | 1 | $0.25 | $0.25 | Mouser | Surtension panneau |
| 8 | Transistor NPN (LED driver) | **2N2222A** | TO-92 | 3 | $0.05 | $0.15 | LCSC | Driver LEDs statut |

### 1.2 Composants Passifs

| # | Composant | Valeur | Package | Qte | Prix unit. | Total | Role |
|---|-----------|--------|---------|-----|-----------|-------|------|
| 9 | Resistance | 68 kΩ ±1% | 0805 | 1 | $0.02 | $0.02 | Diviseur V_panel (R1) |
| 10 | Resistance | 39 kΩ ±1% | 0805 | 1 | $0.02 | $0.02 | Diviseur V_batterie (R3) |
| 11 | Resistances | 10 kΩ ±1% | 0805 | 4 | $0.02 | $0.08 | Diviseurs (R2,R4), pull-up I2C |
| 12 | Resistances | 4.7 kΩ | 0805 | 2 | $0.02 | $0.04 | Pull-up I2C (SDA, SCL) |
| 13 | Resistances | 330 Ω | 0805 | 3 | $0.02 | $0.06 | Limitation courant LEDs |
| 14 | Resistances | 10 Ω 1W | 2512 | 2 | $0.05 | $0.10 | Shunt courant (backup INA219) |
| 15 | Resistances | 100 kΩ | 0805 | 2 | $0.02 | $0.04 | Pull-down MOSFET gate |
| 16 | Condensateur ceramique | 100 nF (0.1µF) | 0805 | 10 | $0.02 | $0.20 | Decouplage alimentation |
| 17 | Condensateur ceramique | 10 µF | 0805 | 4 | $0.03 | $0.12 | Filtrage entree regulateur |
| 18 | Condensateur electrolytique | 100 µF / 25V | Radial 6x11 | 3 | $0.10 | $0.30 | Filtrage puissance |
| 19 | Condensateur electrolytique | 470 µF / 16V | Radial 8x12 | 1 | $0.20 | $0.20 | Filtrage sortie regulateur |
| 20 | Condensateur ceramique | 22 pF | 0805 | 2 | $0.02 | $0.04 | Cristal oscillateur |

### 1.3 Oscillateur et Connectique

| # | Composant | Reference | Qte | Prix unit. | Total | Role |
|---|-----------|-----------|-----|-----------|-------|------|
| 21 | Cristal quartz 8 MHz | HC-49S | 1 | $0.20 | $0.20 | Horloge MCU |
| 22 | LED 3mm verte | Diffuse | 1 | $0.05 | $0.05 | Statut NORMAL |
| 23 | LED 3mm orange | Diffuse | 1 | $0.05 | $0.05 | Statut LOW_BAT |
| 24 | LED 3mm rouge | Diffuse | 1 | $0.05 | $0.05 | Statut CRITICAL |
| 25 | Connecteur a vis 2 pos | 5.08mm | 4 | $0.15 | $0.60 | Panneau, batterie, charge 1-2 |
| 26 | Connecteur a vis 3 pos | 5.08mm | 1 | $0.20 | $0.20 | Charge 3 + masse |
| 27 | Barrette male 2.54mm | 1x4 pins | 2 | $0.10 | $0.20 | UART (TX/RX/VCC/GND), I2C |
| 28 | Barrette male 2.54mm | 1x6 pins | 1 | $0.10 | $0.10 | Connecteur programmation SWD |
| 29 | Fusible rearmable 8A | PTC | 1 | $0.30 | $0.30 | Protection surcourant |
| 30 | Bouton poussoir | 6x6mm | 1 | $0.10 | $0.10 | Reset manuel |

### 1.4 PCB et Boitier

| # | Composant | Specification | Qte | Prix unit. | Total | Fournisseur |
|---|-----------|--------------|-----|-----------|-------|-------------|
| 31 | PCB double face | 80x60mm, FR-4, 1.6mm, HASL | 1 | $1.50 | $1.50 | JLCPCB / PCBWay |
| 32 | Boitier ABS IP54 | 120x80x40mm, gris | 1 | $2.00 | $2.00 | Aliexpress / local |
| 33 | Passe-cables PG7 | Nylon, IP67 | 5 | $0.10 | $0.50 | Local |
| 34 | Entretoise M3 | Nylon, 10mm | 4 | $0.05 | $0.20 | Local |
| 35 | Vis M3x8 | Inox | 8 | $0.02 | $0.16 | Local |

---

## 2. Peripheriques et Modules Optionnels

### 2.1 Affichage

| # | Composant | Reference | Qte | Prix | Role |
|---|-----------|-----------|-----|------|------|
| 36 | LCD 16x2 avec backpack I2C | **PCF8574 + HD44780** | 1 | $2.50 | Affichage etat local |
| 37 | *Alternative* : OLED 0.96" I2C | SSD1306, 128x64 | 1 | $3.00 | Affichage graphique |

### 2.2 Communication (v2.0)

| # | Composant | Reference | Qte | Prix | Role |
|---|-----------|-----------|-----|------|------|
| 38 | Module GSM/GPRS | **SIM800L** | 1 | $4.50 | SMS alertes + donnees |
| 39 | Antenne GSM | SMA, quad-band | 1 | $1.00 | Reception cellulaire |
| 40 | Carte SIM prepayee | Telma / Airtel / Orange | 1 | $1.00 | Abonnement data |

### 2.3 Capteur Temperature (v1.1)

| # | Composant | Reference | Qte | Prix | Role |
|---|-----------|-----------|-----|------|------|
| 41 | Thermistance NTC 10kΩ | B=3950 | 1 | $0.30 | Temp. batterie |
| 42 | Resistance 10kΩ ±1% | 0805 | 1 | $0.02 | Diviseur NTC |

---

## 3. Resume des Couts

### Version v1.0 (Minimum Viable Product)

| Categorie | Cout |
|-----------|------|
| Composants actifs (#1-8) | $5.85 |
| Composants passifs (#9-20) | $1.22 |
| Connectique + oscillateur (#21-30) | $1.85 |
| PCB + boitier (#31-35) | $4.36 |
| LCD I2C (#36) | $2.50 |
| **Total composants v1.0** | **$15.78** |
| Main d'oeuvre assemblage + test | $5.00 |
| **Cout de revient total** | **$20.78** |
| **Prix de vente suggere** | **$49.90** |
| **Marge brute** | **58%** |

### Version v2.0 (avec GSM)

| Categorie | Cout |
|-----------|------|
| v1.0 composants | $15.78 |
| Module GSM SIM800L + antenne + SIM (#38-40) | $6.50 |
| NTC temperature (#41-42) | $0.32 |
| **Total composants v2.0** | **$22.60** |
| Main d'oeuvre | $6.00 |
| **Cout de revient total** | **$28.60** |
| **Prix de vente suggere** | **$64.90** |
| **Marge brute** | **56%** |

---

## 4. Outils de Developpement (Logiciel)

### 4.1 Essentiels (gratuits)

| Outil | Version | Licence | Utilisation |
|-------|---------|---------|-------------|
| **Keil MDK-Lite** | v5.38+ | Gratuit (32kB code) | IDE + compilateur + simulateur |
| **STM32CubeMX** | 6.x | Gratuit | Configuration MCU + generation code init |
| **Git** | 2.x+ | Open source | Gestion de version |
| **VS Code** | Latest | Gratuit | Edition code + Markdown |
| **PuTTY / minicom** | Latest | Gratuit | Terminal serie UART |

### 4.2 Conception PCB

| Outil | Licence | Utilisation |
|-------|---------|-------------|
| **KiCad** | Open source (gratuit) | Schema + routage PCB |
| *Alternative* : EasyEDA | Gratuit (cloud) | Schema + commande PCB directe |

### 4.3 Documentation

| Outil | Utilisation |
|-------|-------------|
| **Obsidian** / VS Code | Lecture/edition Markdown |
| **draw.io** | Diagrammes et schemas |
| **LibreOffice Calc** | Tableurs BOM et budgets |

---

## 5. Outils Physiques (Atelier)

### 5.1 Outillage essentiel

| Outil | Specification | Prix approx. | Obligatoire |
|-------|--------------|-------------|-------------|
| **Fer a souder** | Reglable, 60W, pointe fine | $15-30 | Oui |
| **Etain de soudure** | 0.8mm, Sn63/Pb37 ou sans plomb | $5 | Oui |
| **Multimetre numerique** | Mesure V/A/Ω, diode, continuite | $15-25 | Oui |
| **Alimentation labo** | 0-30V, 0-5A, reglable | $35-60 | Oui |
| **Pince coupante** | Precision electronique | $5 | Oui |
| **Pince a denuder** | AWG 22-30 | $5 | Oui |
| **Tournevis precision** | Kit cruciforme + plat | $5 | Oui |
| **Loupe / lampe** | Eclairante, x3-x5 | $10 | Recommande |
| **Tapis antistatique** | ESD safe | $8 | Recommande |

### 5.2 Outillage avance (pour la production)

| Outil | Specification | Prix approx. |
|-------|--------------|-------------|
| Oscilloscope numerique | 2 voies, 50 MHz min | $80-200 |
| Analyseur logique | 8 canaux, USB | $15-30 |
| Programmateur ST-Link V2 | USB, SWD | $3-8 |
| Station de soudure CMS | Air chaud + fer | $50-100 |
| Convertisseur USB-UART | CP2102 ou CH340 | $2-3 |

### 5.3 Materiel pour tests terrain

| Materiel | Specification | Prix approx. |
|----------|--------------|-------------|
| Panneau solaire 12V/50W | Polycristallin, avec cable MC4 | $25-35 |
| Batterie plomb 12V/7Ah | Scelle, sans entretien (test) | $12-18 |
| Ampoules LED 12V | 3W, 5W (simulation charges) | $2-5 |
| Cable solaire 4mm² | Rouge + noir, 5m chaque | $5 |
| Connecteurs MC4 | Male + femelle, paire | $2 |

---

## 6. Fournisseurs Recommandes

### International (commande en ligne)

| Fournisseur | Specialite | Delai Madagascar | Livraison |
|-------------|-----------|-----------------|-----------|
| **LCSC** (lcsc.com) | Composants electroniques, prix bas | 2-4 semaines | $5-15 |
| **JLCPCB** (jlcpcb.com) | Fabrication PCB | 2-3 semaines | $2-8 |
| **Mouser** (mouser.com) | Composants, large stock | 3-5 semaines | $15-25 |
| **Digikey** (digikey.com) | Composants, datasheets | 3-5 semaines | $15-25 |
| **Aliexpress** | Modules, boitiers, outils | 3-6 semaines | $1-5 |

### Local Madagascar

| Fournisseur | Localisation | Produits |
|-------------|-------------|----------|
| Quincailleries Analakely | Antananarivo centre | Connecteurs, fils, boitiers |
| Marche Isotry | Antananarivo | Composants electroniques basiques |
| Shoprite / Jumbo Score | National | Piles, multimetres basiques |
| Techno-Shop Tana | Antananarivo | Outillage, fers a souder |

---

## 7. Checklist de Demarrage Projet

### Phase Prototype ($50-100 budget)

- [ ] Commander STM32F030F4P6 (x5 pour marge) — $4.00
- [ ] Commander INA219 (x4) — $4.00
- [ ] Commander composants passifs kit — $5.00
- [ ] Commander PCB chez JLCPCB — $7.00 (5 pieces)
- [ ] Installer Keil MDK-Lite sur PC
- [ ] Installer STM32CubeMX
- [ ] Acheter programmateur ST-Link V2 — $5.00
- [ ] Acheter convertisseur USB-UART — $3.00
- [ ] Acheter panneau solaire 12V test — $30.00
- [ ] Acheter batterie 12V/7Ah test — $15.00
- [ ] Assembler le premier prototype
- [ ] Flasher le firmware et tester
- [ ] Valider sur 5 sites pilotes

### Budget prototype minimal

```
Composants electroniques :  $20
PCB (5 pieces) :            $7
Outils (fer, multimetre) :  $35
Panneau + batterie test :   $45
Programmateur + USB-UART :  $8
───────────────────────────────
Total prototype :           ~$115
```

---

[Retour au pitch](./00-PITCH.md) | [Retour a l'index projets](../00-INDEX-PROJET.md)
