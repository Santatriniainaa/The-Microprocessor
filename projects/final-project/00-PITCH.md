# TSAKITSAKY ENERGY - Systeme Intelligent de Gestion d'Energie Solaire

## *Optimiser chaque watt pour Madagascar*

> **Auteur** : Santatriniaina RAZAFINDRAKOTO
> **Date** : 20 mars 2026
> **Plateforme** : ARM Cortex-M0 / Keil MDK

---

## Le Probleme a Madagascar

```
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║   85% des menages ruraux n'ont PAS d'acces a l'electricite      ║
║                                                                  ║
║   La JIRAMA coupe le courant 4 a 12h/jour en milieu urbain      ║
║                                                                  ║
║   Le solaire est abondant : 2800 h/an d'ensoleillement          ║
║   mais les systemes existants sont mal geres et gaspillent      ║
║   40-60% de l'energie produite                                   ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝
```

## La Solution : TSAKITSAKY ENERGY

Un **controleur intelligent** embarque sur ARM Cortex-M0 qui :

1. **Mesure** la production solaire (panneau) et la consommation (charges)
2. **Optimise** la distribution d'energie vers les charges par priorite
3. **Protege** la batterie contre la surcharge et la decharge profonde
4. **Communique** l'etat du systeme via UART (ecran LCD ou SMS via module GSM)
5. **Apprend** les habitudes de consommation pour anticiper les besoins

```
   ☀ Soleil
     │
 ┌───▼────┐     ┌──────────────────┐     ┌─────────────┐
 │Panneau │────→│  TSAKITSAKY      │────→│ Charges     │
 │Solaire │ ADC │  CONTROLEUR      │ GPIO│ Priorite 1  │ Eclairage
 │12V/50W │     │  (Cortex-M0)     │────→│ Priorite 2  │ Telephone
 └────────┘     │                  │────→│ Priorite 3  │ Radio/TV
                │  ┌────┐ ┌─────┐ │     └─────────────┘
 ┌──────────┐   │  │MPPT│ │Stats│ │
 │ Batterie │←─→│  └────┘ └─────┘ │────→ UART → LCD/GSM
 │ 12V/100Ah│ADC│                  │
 └──────────┘   └──────────────────┘
```

---

## Marche et Rentabilite

### TAM/SAM/SOM (Marche adressable)

| Niveau | Marche | Valeur estimee |
|--------|--------|---------------|
| **TAM** | Systemes solaires off-grid Afrique | $3.2 milliards |
| **SAM** | Kits solaires intelligents Madagascar + Ocean Indien | $180 millions |
| **SOM** | Marche captable 3 premieres annees | $5-15 millions |

### Cout unitaire vs Prix de vente

| Composant | Cout (USD) |
|-----------|-----------|
| MCU STM32 (Cortex-M0) | $0.80 |
| Capteurs courant/tension (INA219 x2) | $2.00 |
| MOSFET de commutation (x3 charges) | $1.50 |
| PCB + composants passifs | $3.00 |
| Boitier IP65 | $2.00 |
| Module UART/LCD | $3.00 |
| **Total composants** | **$12.30** |
| Assemblage + test | $5.00 |
| **Cout total** | **$17.30** |
| **Prix de vente** | **$45-65** |
| **Marge brute** | **60-73%** |

### Projection financiere

| Annee | Unites vendues | CA (USD) | Marge nette |
|-------|---------------|----------|-------------|
| A1 | 2 000 | $100 000 | -$30 000 (investissement) |
| A2 | 10 000 | $500 000 | $120 000 |
| A3 | 30 000 | $1 500 000 | $450 000 |
| A5 | 100 000 | $4 500 000 | $1 800 000 |

### Avantage concurrentiel

| Critere | Solutions existantes | TSAKITSAKY |
|---------|---------------------|------------|
| Prix | $80-200 | **$45-65** |
| Intelligence | Charge ON/OFF basique | **MPPT + priorite + stats** |
| Protection batterie | Coupure brutale | **Decharge progressive** |
| Monitoring | Aucun | **UART + LCD + SMS** |
| Fabrication | Importe (Chine) | **Assemblage local** |
| Maintenance | Remplacement | **Diagnostic a distance** |

---

## Documentation technique

| Document | Description |
|----------|------------|
| [01-Architecture.md](./01-Architecture.md) | Architecture systeme complete |
| [02-Business-Plan.md](./02-Business-Plan.md) | Plan d'affaires detaille |
| [03-Hardware-Design.md](./03-Hardware-Design.md) | Schema electronique et BOM |
| [04-Software-Architecture.md](./04-Software-Architecture.md) | Architecture logicielle |
| [05-Codebase.md](./05-Codebase.md) | Code source complet (12 fichiers C) |
| [06-Tests-Validation.md](./06-Tests-Validation.md) | Plan de tests et validation |
| [07-Deploiement.md](./07-Deploiement.md) | Strategie de deploiement Madagascar |

---

> *"Ny herin'ny masoandro ho an'ny rehetra"*
> *La puissance du soleil pour tous*
