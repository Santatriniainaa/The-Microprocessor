#*The Microprocessor: A Practical Introduction Using The Arm Cortex-M0 Processor*

Ce document propose un parcours structuré pour étudier le livre de Darshak S. Vasavada et S. K. Sinha. Il s’adresse aux débutants souhaitant comprendre le fonctionnement interne d’un microprocesseur, en particulier l’architecture ARM Cortex-M0, à la fois du point de vue matériel et logiciel.

---

## Prérequis
- Connaissances de base en électronique numérique (bits, octets, logique binaire).
- Notions élémentaires de programmation (variables, boucles, fonctions) seraient un plus, mais le livre reprend les bases.

---

## Structure générale du livre
Le livre se divise en quatre grandes parties :

1. **Fondamentaux du microprocesseur** (chapitres 1 à 3) – architecture, exécution des instructions.
2. **Programmation assembleur ARM Cortex-M0** (chapitres 4 à 11) – instructions, branchements, pile, exceptions.
3. **Programmation en C pour systèmes embarqués** (chapitres 12 à 16) – types de données, fonctions, compilation.
4. **Microcontrôleur et applications** (chapitres 17 à 19) – périphériques, entrées-sorties, exemples concrets.

Les annexes fournissent des informations complémentaires sur l’environnement de développement, le code de démarrage, etc.

---

## Plan d’étude détaillé

### Partie 1 : Les bases du microprocesseur (Chapitres 1–3)

| Chapitre | Sujet | Objectifs d’apprentissage | Exercices clés |
|----------|-------|---------------------------|----------------|
| 1 | Microprocessor System | Comprendre les composants d’un système à microprocesseur : CPU, mémoire, bus, ports E/S, horloge, reset. | Aucun exercice pratique dans ce chapitre, mais il pose les bases. |
| 2 | Instruction Set Architecture | Découvrir les registres du Cortex-M0, le jeu d’instructions (arithmétique, mémoire, contrôle), la structure des instructions. | Observer les formats d’instructions et leur encodage. |
| 3 | Program Execution | Savoir comment une instruction est fetchée, décodée et exécutée dans une architecture RISC, y compris le pipeline. | Suivre le déroulement d’un programme simple dans le simulateur. |

**Conseil** : Pour le chapitre 3, utiliser le simulateur Keil pour visualiser l’exécution pas à pas et les changements dans les registres.

---

### Partie 2 : Programmation en assembleur (Chapitres 4–11)

Chaque chapitre de cette partie propose plusieurs exercices pratiques. Il est essentiel de les réaliser dans l’ordre.

| Chapitre | Sujet | Concepts abordés | Exercices |
|----------|-------|------------------|-----------|
| 4 | Assembly Programming | Structure d’un programme assembleur : directives, sections, labels, premier programme. | 4.1 à 4.3 : création d’un projet, premier programme, accès mémoire. |
| 5 | Arithmetic Operations | Instructions arithmétiques, nombres signés/non signés, flags APSR, comparaisons. | 5.1 à 5.7 : additions, multiplications, tests de flags. |
| 6 | Bit-Level Operations | Opérations booléennes, manipulation de bits, décalages, extension de mot. | 6.1 à 6.5 : ET/OU, décalages logiques/arithmétiques, extensions. |
| 7 | Load and Store Operations | Alignement, endianness, accès mémoire (byte, half-word, word), adressage avec offset. | 7.1 à 7.6 : visualisation des données, accès avec offset. |
| 8 | Branch and Subroutine | Branchements conditionnels et inconditionnels, appels de sous-programmes. | 8.1 à 8.5 : boucles, sous-programmes, imbrication. |
| 9 | Stack Operations | Rôle de la pile, instructions PUSH/POP, frames de pile, variables locales. | 9.1 à 9.5 : observation de la pile, sauvegarde de registres. |
| 10 | Exceptions | Modèle d’exception, table des vecteurs, reset, HardFault, empilement d’exception. | 10.1 à 10.4 : écriture d’un vecteur, déclenchement d’une faute. |
| 11 | SysTick and Core Interrupts | Timer SysTick, interruptions, masquage, NMI. | 11.1 à 11.5 : programmation du timer, interruption, masquage. |

**Remarque** : Les exercices se font avec le simulateur intégré de Keil MDK. Un modèle de projet est fourni sur le site compagnon.

---

### Partie 3 : Programmation en C (Chapitres 12–16)

Ces chapitres montrent comment les constructions du langage C sont traduites en assembleur Cortex-M0. Il est utile de compiler les exemples et d’examiner le code désassemblé.

| Chapitre | Sujet | Points clés | Exercices |
|----------|-------|-------------|-----------|
| 12 | Introduction to C Programming | Structure d’un programme C, fonctions, variables, fichiers d’en-tête. | 12.1 à 12.5 : création de projet C, appels de fonctions, variables globales/locales, utilisation de header. |
| 13 | Basic Data Types | Types char, int, float, représentation, endianness, opérations arithmétiques et bit à bit. | 13.1 à 13.9 : affichage en mémoire, opérations sur entiers et flottants, transtypage. |
| 14 | Functions | Passage d’arguments, retour, variables locales, branchements conditionnels, boucles. | 14.1 à 14.6 : analyse du code généré pour if, for, switch. |
| 15 | Extended Data Types | Tableaux, structures, pointeurs, arithmétique de pointeurs. | 15.1 à 15.9 : accès aux tableaux, structures, pointeurs non initialisés. |
| 16 | Compilation Process | Préprocessing, compilation, assemblage, édition de liens, fichier map. | 16.1 à 16.5 : observation des fichiers intermédiaires, map file. |

**Conseil** : Comparer systématiquement le code C avec le désassemblage pour bien comprendre la génération de code.

---

### Partie 4 : Microcontrôleur et applications (Chapitres 17–19)

Ces chapitres utilisent un microcontrôleur abstrait (AM0) pour expliquer les périphériques et la programmation d’entrées-sorties.

| Chapitre | Sujet | Contenu | Exemples |
|----------|-------|---------|----------|
| 17 | Microcontroller | Schéma bloc, horloge, gestion d’alimentation, registres périphériques, API de bas niveau. | Définition des registres, accès via pointeurs. |
| 18 | IO Programming | Programmation des GPIO, timer, ADC, DAC, UART, interruptions. | 18.1 à 18.6 : lecture de bouton, clignotement de LED, conversion analogique, communication série. |
| 19 | Microprocessor Applications | Applications réelles : contrôle de luminosité de LED, capteur de lumière, mesure de température, régulation de moteur. | Codes complets intégrant capteurs, actionneurs et gestion d’énergie. |

**Note** : Ces exemples ne tournent pas sur le simulateur mais peuvent être portés sur une carte réelle. Ils illustrent les concepts clés des systèmes embarqués.

---

## Conseils pour une étude efficace

1. **Progresser pas à pas** : Ne sautez pas d’exercice, chaque chapitre s’appuie sur le précédent.
2. **Expérimenter** : Modifiez les programmes, changez les valeurs, observez les résultats dans les registres et la mémoire.
3. **Utiliser le simulateur** : Il permet de visualiser en détail l’exécution sans matériel.
4. **Prendre des notes** : Sur les flags, les modes d’adressage, les conventions d’appel (AAPCS).
5. **Pour la partie C** : Compilez avec différentes optimisations (-O0, -Os) et observez les différences dans le code assembleur généré.

---

## Calendrier indicatif

- **Semaines 1–2** : Chapitres 1 à 4 (bases et premier assembleur).
- **Semaines 3–4** : Chapitres 5 à 7 (arithmétique, bits, chargement).
- **Semaines 5–6** : Chapitres 8 à 9 (branchements, pile).
- **Semaine 7** : Chapitres 10 à 11 (exceptions, interruptions).
- **Semaines 8–9** : Chapitres 12 à 14 (C : bases, fonctions).
- **Semaine 10** : Chapitres 15 à 16 (types avancés, compilation).
- **Semaines 11–12** : Chapitres 17 à 19 (microcontrôleur, applications).

@Santatriniaina RAZAFINDRAKOTO
