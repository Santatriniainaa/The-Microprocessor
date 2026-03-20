# 02 - Business Plan : TSAKITSAKY ENERGY

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

---

## 1. Resume Executif

**TSAKITSAKY ENERGY** developpe un controleur solaire intelligent a bas cout ($45-65) pour les menages et commerces malgaches hors reseau. Le produit optimise l'utilisation de l'energie solaire grace a un microcontroleur ARM Cortex-M0, permettant aux utilisateurs d'economiser **40-60% d'energie** par rapport aux regulateurs basiques.

---

## 2. Analyse du Marche Malgache

### 2.1 Contexte energetique

| Indicateur | Valeur |
|-----------|--------|
| Population | 30 millions (2026) |
| Taux d'electrification rural | **15%** |
| Taux d'electrification urbain | **67%** |
| Coupures JIRAMA moyennes | **4-12h/jour** |
| Ensoleillement annuel | **2800 heures** |
| Kits solaires en usage | ~500 000 |
| Croissance marche solaire | **+25%/an** |

### 2.2 Segments cibles

| Segment | Taille | Prix acceptable | Besoin cle |
|---------|--------|----------------|-----------|
| **Menages ruraux** | 4M foyers | $45-55 | Eclairage + charge telephone |
| **Epiceries/Hotely** | 200K | $55-65 | Eclairage + frigo + TV |
| **Ecoles/CSB** | 50K | $55-65 | Eclairage + ordinateur |
| **ONG / Projets dev** | - | $45-65 (volume) | Deploiement massif |

### 2.3 Concurrence a Madagascar

| Concurrent | Prix | Intelligence | Fabrication | Faiblesses |
|-----------|------|-------------|-------------|-----------|
| Regulateurs chinois generiques | $15-30 | Aucune | Chine | Pas de protection batterie, gaspillage |
| M-KOPA (Kenya) | $200+ | Cloud (GSM) | Kenya | Trop cher, Pay-as-you-go complexe |
| BBOXX | $150+ | Cloud | UK/Chine | Trop cher pour Madagascar |
| **TSAKITSAKY** | **$45-65** | **Locale (MCU)** | **Madagascar** | MVP a valider |

---

## 3. Modele Economique

### 3.1 Revenue streams

```
┌────────────────────────────────────────────┐
│           Sources de Revenus                │
├──────────────────┬─────────────────────────┤
│ Vente hardware   │ 70% du CA               │
│ (controleur)     │ $45-65 par unite        │
├──────────────────┼─────────────────────────┤
│ Installation +   │ 15% du CA               │
│ Formation        │ $10-15 par installation │
├──────────────────┼─────────────────────────┤
│ Maintenance      │ 10% du CA               │
│ annuelle         │ $5-10/an par client     │
├──────────────────┼─────────────────────────┤
│ Data / Analytics │ 5% du CA (futur)        │
│ (agregation)     │ Vente aux ONG/gouv      │
└──────────────────┴─────────────────────────┘
```

### 3.2 Canaux de distribution

| Canal | Couverture | Commission |
|-------|-----------|------------|
| **Revendeurs locaux** (quincailleries) | 22 regions | 15-20% |
| **Techniciens solaires** independants | Villages | 10-15% + formation |
| **ONG partenaires** (WWF, USAID, GIZ) | Projets | Prix volume |
| **En ligne** (Facebook, site web) | Urbain | Direct |

### 3.3 Structure de couts

| Poste | A1 | A2 | A3 |
|-------|-----|-----|-----|
| Composants (BOM) | $34 600 | $130 000 | $345 000 |
| Main d'oeuvre assemblage | $15 000 | $40 000 | $90 000 |
| Loyer atelier (Tana) | $6 000 | $12 000 | $18 000 |
| Marketing | $20 000 | $30 000 | $50 000 |
| Salaires (equipe 4→12) | $36 000 | $72 000 | $144 000 |
| Logistique/distribution | $8 000 | $25 000 | $60 000 |
| R&D (v2, v3) | $10 000 | $20 000 | $40 000 |
| **Total depenses** | **$129 600** | **$329 000** | **$747 000** |
| **CA** | **$100 000** | **$500 000** | **$1 500 000** |
| **Resultat net** | **-$29 600** | **$171 000** | **$753 000** |

---

## 4. Plan de Deploiement

### Phase 1 : Prototype + Validation (Mois 1-6)
- Developper le firmware sur Keil MDK (ce projet)
- Concevoir le PCB v1
- Fabriquer 20 prototypes
- Tester dans 5 foyers pilotes (Analamanga)
- Valider les gains energetiques

### Phase 2 : Lancement commercial (Mois 7-12)
- Production batch de 500 unites
- Former 20 techniciens installateurs
- Lancer dans 3 regions : Analamanga, Vakinankaratra, Atsinanana
- Partenariat avec 2 ONG

### Phase 3 : Scaling (Annee 2-3)
- 22 regions de Madagascar
- Production de 2000 unites/mois
- Version v2 avec module GSM integre
- Expansion : Comores, Maurice, Reunion

---

## 5. Impact Social

| Indicateur | Objectif A3 |
|-----------|------------|
| Menages electrifies | 30 000 |
| Emplois crees (direct + indirect) | 150 |
| Reduction CO2 (vs kerosene) | 2 000 tonnes/an |
| Heures d'eclairage supplementaires | 4h/jour/menage |
| Economies pour les menages | $15-30/mois (vs bougies+piles) |

---

## 6. Equipe Necessaire

| Role | Profil | Salaire mensuel (MGA) |
|------|--------|----------------------|
| **CEO / Ingenieur firmware** | Ingenieur electronique (auteur) | 2 000 000 |
| **CTO / Hardware** | Ingenieur electronique/PCB | 1 800 000 |
| **Business Developer** | Commercial terrain | 1 200 000 |
| **Technicien assemblage** (x2) | Technicien electronique | 800 000 |

---

## 7. Financement Recherche

| Source | Montant cible | Statut |
|--------|--------------|--------|
| Fonds propres + famille | $5 000 | A mobiliser |
| Concours Orange Solidarite (Madagascar) | $10 000 | A postuler |
| FIHARIANA (programme presidence) | $15 000 | A postuler |
| GIZ / EnDev Madagascar | $20 000 | A postuler |
| Investisseur impact (Miarakap, Oikocredit) | $50 000 | Serie seed A2 |

---

[<< Architecture](./01-Architecture.md) | [Hardware Design >>](./03-Hardware-Design.md)
