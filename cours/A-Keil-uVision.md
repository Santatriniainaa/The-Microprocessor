# Annexe A : Environnement de Developpement Keil uVision

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 19](./19-Serie-UART.md)

---

## Objectifs

> Configurer et utiliser Keil MDK avec uVision IDE pour developper, compiler et deboguer des programmes ARM Cortex-M0.

---

## A.1 Keil MDK

### Composants

| Composant | Role |
|-----------|------|
| **uVision IDE** | Editeur, gestionnaire de projet, debugger |
| **armclang** | Compilateur/assembleur ARM integre |
| **armlink** | Editeur de liens |
| **Debugger** | Debogage avec simulateur ou hardware |
| **Simulateur** | Simulation Cortex-M0 integree (pas besoin de carte) |

### Versions

| Version | Limite | Prix |
|---------|--------|------|
| MDK-Lite | 32 kB de code | Gratuit |
| MDK-Essential | Illimite | Payant |
| MDK-Professional | + middleware | Payant |

---

## A.2 Creation d'un Projet

1. **Project → New uVision Project**
2. Choisir un dossier et un nom de projet
3. **Selectionner le microcontroleur** : ARM → ARM Cortex-M0 (ou un MCU specifique)
4. Decliner l'ajout automatique de fichiers startup (ou accepter)
5. **Ajouter les fichiers source** : clic droit sur Source Group → Add Existing/New Files
   - `startup.s` (code de demarrage)
   - `exercise.s` ou `main.c` (votre code)

---

## A.3 Configuration du Projet

### Options (Alt+F7 ou Project → Options for Target)

| Onglet | Parametres cles |
|--------|----------------|
| **Target** | Frequence cristal, utilisation du simulateur |
| **Output** | Nom du fichier executable, generer .hex |
| **Listing** | Generer fichier .map |
| **C/C++** | Niveau d'optimisation (-O0 pour debug) |
| **Asm** | Options de l'assembleur |
| **Linker** | Scatter file (placement memoire) |
| **Debug** | Choisir : Simulateur ou hardware (ST-Link, J-Link...) |

### Configuration debug recommandee

```
Debug → Use Simulator ✓
       → Run to main() ✓  (ou configurer debug.ini)
```

---

## A.4 Compilation et Construction

| Action | Raccourci | Description |
|--------|-----------|-------------|
| **Build** | `F7` | Compile les fichiers modifies et lie |
| **Rebuild All** | - | Recompile tout depuis zero |

### Fenetre Build Output

```
Build target 'Target 1'
assembling startup.s...
assembling exercise.s...
linking...
Program Size: Code=148 RO-data=236 RW-data=4 ZI-data=1024
"project.axf" - 0 Error(s), 0 Warning(s).
```

### Fichiers generes

| Extension | Contenu |
|-----------|---------|
| `.o` | Fichier objet (par fichier source) |
| `.axf` | Executable (charge par le debugger) |
| `.hex` | Format Intel HEX (pour programmer la Flash) |
| `.map` | Carte memoire detaillee |

---

## A.5 Debogage

### Demarrer le debugger

| Action | Raccourci |
|--------|-----------|
| Start/Stop Debug Session | `Ctrl+F5` |
| Step (pas a pas) | `F11` |
| Step Over | `F10` |
| Run (executer) | `F5` |
| Stop | `Esc` |
| Reset | - (bouton RST) |

### Fenetres du debugger

| Fenetre | Contenu |
|---------|---------|
| **Source Window** | Code source avec fleche d'execution |
| **Disassembly** | Code machine desassemble |
| **Registers** | R0-R15, APSR, xPSR |
| **Memory** | Contenu de la memoire (saisir adresse) |
| **Watch** | Variables surveillees |
| **Call Stack** | Pile d'appels de fonctions |

### Examiner la memoire

- **View → Memory Window**
- Saisir l'adresse (ex: `0x20000000`) ou un symbole (ex: `x`)
- Choisir le format : Unsigned Long, Hex, etc.

### Examiner les registres

La fenetre Registers montre en temps reel :
- **R0-R12** : registres a usage general
- **R13 (SP)** : pointeur de pile
- **R14 (LR)** : registre de lien
- **R15 (PC)** : compteur de programme (adresse courante)
- **xPSR** : drapeaux N, Z, C, V

---

## A.6 Points d'Arret (Breakpoints)

| Type | Comment |
|------|---------|
| Point d'arret simple | Double-clic dans la marge du code source |
| Supprimer | Double-clic a nouveau |
| Desactiver tous | Debug → Breakpoints → Kill All |

---

## A.7 Fichier debug.ini

Script execute au demarrage du debugger :

```
// debug.ini - Executer automatiquement jusqu'a asm_main
g, asm_main     // Run to asm_main
```

Configuration : **Debug → Initialization File → debug.ini**

---

## A.8 Astuces et Bonnes Pratiques

| Probleme | Solution |
|----------|---------|
| Code modifie mais pas d'effet | **Toujours Rebuild** avant de debugger |
| Disassembly montre l'ancien code | Quitter debug, rebuild, puis re-entrer |
| Registres pas mis a jour | Single-step (F11) pour voir les changements |
| Programme plante (HardFault) | Examiner PC dans le handler, verifier alignement |
| Boucle infinie inattendue | Verifier le code desassemble, pas le source |

> **Regle d'or** : Toujours **quitter le mode debug** → **modifier le code** → **rebuild** → **re-entrer en debug**. Ne jamais modifier le code pendant le debug !

---

## Resume

- **Keil MDK** = uVision IDE + armclang + debugger + simulateur
- **Workflow** : Editer → Build (F7) → Debug (Ctrl+F5) → Step (F11)
- **Fenetres** : Source, Disassembly, Registers, Memory, Watch
- **Fichiers** : .s/.c (source) → .o (objet) → .axf (executable) → .hex (Flash)
- **Toujours** rebuild apres modification avant de debugger

---

[<< Chapitre 19](./19-Serie-UART.md) | [Retour a l'index](./00-INDEX.md)
