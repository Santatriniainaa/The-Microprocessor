# 07 - Strategie de Deploiement a Madagascar

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

---

## 1. Roadmap Produit

```
     Q1-Q2 2026              Q3-Q4 2026              2027                 2028
 ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐    ┌──────────────┐
 │ v1.0 PROTOTYPE  │    │ v1.1 PRODUCTION │    │ v2.0 GSM    │    │ v3.0 CLOUD   │
 │                 │    │                 │    │             │    │              │
 │ - Firmware Keil │───→│ - PCB v1 fabr.  │───→│ - Module    │───→│ - Dashboard  │
 │ - Simulation    │    │ - 500 unites    │    │   SIM800    │    │   web        │
 │ - 5 sites test  │    │ - 3 regions     │    │ - SMS alert │    │ - Analytics  │
 │ - Validation    │    │ - 20 installat. │    │ - 22 regions│    │ - Ocean Indien│
 └─────────────────┘    └─────────────────┘    └─────────────┘    └──────────────┘
   Budget: $5K             Budget: $50K          Budget: $120K      Budget: $300K
```

## 2. Zones de Deploiement Prioritaires

| Phase | Region | Population rurale | Taux electrif. | Potentiel |
|-------|--------|-------------------|---------------|-----------|
| **1** | Analamanga | 2.5M | 35% | Proximite Tana, logistique facile |
| **1** | Vakinankaratra | 1.8M | 20% | Agriculture, revenus moyens |
| **1** | Atsinanana | 1.2M | 15% | Cote est, fort ensoleillement |
| **2** | Atsimo-Andrefana | 1.5M | 8% | Grand sud, tres fort ensoleillement |
| **2** | Boeny | 0.8M | 12% | Tourisme, hotels |
| **2** | DIANA | 0.6M | 15% | Nosy Be, eco-tourisme |
| **3** | 16 autres regions | 20M | 5-15% | Expansion nationale |

## 3. Partenaires Strategiques

| Partenaire | Role | Avantage |
|-----------|------|----------|
| **ADER** (Agence Dev. Electrif. Rurale) | Cadre reglementaire, subventions | Acces aux projets gouvernementaux |
| **GIZ EnDev** | Financement, expertise technique | $20-50K grants, reseau international |
| **WWF Madagascar** | Deploiement communautaire | Acces aux villages, credibilite |
| **Orange Money** | Paiement mobile (Pay-as-you-go) | Faciliter l'achat pour les menages |
| **INSCAE / Polytechnique** | Stagiaires, R&D | Main d'oeuvre qualifiee, innovation |

## 4. Chaine d'Approvisionnement

```
 Composants (Mouser/Digikey/LCSC)
       │ Commande en ligne (2-4 semaines)
       ▼
 Atelier Antananarivo (Analakely/67ha)
       │ Assemblage + Test + Programmation
       │ Capacite : 200 unites/mois (phase 1)
       ▼
 Entrepot regional (3 depots)
       │ Analamanga / Vakinankaratra / Atsinanana
       ▼
 Revendeurs locaux + Techniciens
       │ 50+ points de vente
       ▼
 Installation chez le client
   Formation 30 min
```

## 5. Formation des Techniciens

| Module | Duree | Contenu |
|--------|-------|---------|
| Installation | 2h | Montage panneau, cablage, mise en service |
| Diagnostic | 2h | Lecture LED status, menu UART, depannage |
| Maintenance | 1h | Nettoyage panneau, verification batterie |
| Commercial | 1h | Argumentaire, demonstration, SAV |
| **Total** | **6h** | Certification "Technicien TSAKITSAKY" |

## 6. Indicateurs de Suivi (KPIs)

| KPI | Objectif A1 | Objectif A2 | Objectif A3 |
|-----|------------|------------|------------|
| Unites installees | 2 000 | 12 000 | 42 000 |
| Taux de panne | < 5% | < 3% | < 2% |
| Satisfaction client | > 80% | > 85% | > 90% |
| Delai installation | < 2h | < 1.5h | < 1h |
| CA mensuel | $8K | $42K | $125K |
| Points de vente | 20 | 80 | 200 |

## 7. Risques et Mitigations

| Risque | Impact | Probabilite | Mitigation |
|--------|--------|-------------|-----------|
| Retard composants | Elevé | Moyen | Stock de securite 3 mois, 2 fournisseurs |
| Concurrence chinoise | Moyen | Elevé | Differenciation par l'intelligence + SAV local |
| Instabilite politique | Elevé | Faible | Diversification geographique rapide |
| Vol/casse sur terrain | Moyen | Moyen | Boitier robuste IP54, fixation securisee |
| Defaut batterie client | Moyen | Elevé | Formation + diagnostic automatique |

---

## 8. Impact et Vision

### A 5 ans

```
┌──────────────────────────────────────────────────────┐
│                                                       │
│   100 000 foyers electrifies a Madagascar             │
│   150 emplois directs + 500 indirects                 │
│   2 000 tonnes CO2 evitees / an                       │
│   Expansion : Comores, Maurice, Mozambique            │
│   Produit phare du "Made in Madagascar" tech          │
│                                                       │
└──────────────────────────────────────────────────────┘
```

> *"Tsy misy mahasoa noho ny fahazavana"*
> *Rien n'est plus precieux que la lumiere*

---

[<< Tests](./06-Tests-Validation.md) | [Retour au pitch](./00-PITCH.md)
