# Chapitre 6 : Operations au Niveau Bit

[Retour a l'index](./00-INDEX.md) | [<< Chapitre 5](./05-Operations-Arithmetiques.md) | [Chapitre 7 >>](./07-Acces-Memoire.md)

---

## Objectifs

> Maitriser les operations booleennes, le masquage de bits, les decalages et les manipulations au niveau bit sur le Cortex-M0.

---

## 6.1 Operations Booleennes

### Tables de verite

| A | B | A AND B | A OR B | A XOR B | NOT A |
|---|---|---------|--------|---------|-------|
| 0 | 0 | 0 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 1 | 1 |
| 1 | 0 | 0 | 1 | 1 | 0 |
| 1 | 1 | 1 | 1 | 0 | 0 |

### Instructions Cortex-M0

| Instruction | Syntaxe | Operation |
|------------|---------|-----------|
| `ANDS` | `ANDS Rdn, Rm` | Rdn ← Rdn AND Rm |
| `ORRS` | `ORRS Rdn, Rm` | Rdn ← Rdn OR Rm |
| `EORS` | `EORS Rdn, Rm` | Rdn ← Rdn XOR Rm |
| `MVNS` | `MVNS Rd, Rm` | Rd ← NOT Rm |
| `BICS` | `BICS Rdn, Rm` | Rdn ← Rdn AND (NOT Rm) |
| `TST` | `TST Rn, Rm` | Rn AND Rm (drapeaux seulement) |

---

## 6.2 Masquage de Bits

### Mettre des bits a 0 (AND avec masque)

```
Valeur :   1011 0110 1101 0011
Masque :   1111 1111 0000 1111   (0 ou on veut effacer)
AND :      1011 0110 0000 0011   ← bits 4-7 effaces
```

```asm
movs r1, #0xF0        // masque = bits a effacer
mvns r1, r1           // inverser → 0xFFFFFF0F
ands r0, r1           // effacer bits 4-7 de r0
```

### Mettre des bits a 1 (OR avec masque)

```
Valeur :   1011 0110 0000 0011
Masque :   0000 0000 1111 0000   (1 ou on veut activer)
OR :       1011 0110 1111 0011   ← bits 4-7 actives
```

```asm
movs r1, #0xF0        // masque = bits a activer
orrs r0, r1           // activer bits 4-7 de r0
```

### Inverser des bits (XOR avec masque)

```
Valeur :   1011 0110 1101 0011
Masque :   0000 0000 1111 0000   (1 ou on veut inverser)
XOR :      1011 0110 0010 0011   ← bits 4-7 inverses
```

```asm
movs r1, #0xF0
eors r0, r1           // inverser bits 4-7 de r0
```

### Tester des bits (TST = AND sans modification)

```asm
movs r1, #0x04        // masque pour bit 2
tst  r0, r1           // teste bit 2 de r0
beq  bit_est_zero     // Z=1 si bit 2 = 0
bne  bit_est_un       // Z=0 si bit 2 = 1
```

### Effacer des bits (BIC = AND NOT)

```asm
movs r1, #0xF0
bics r0, r1           // r0 = r0 AND (NOT 0xF0) → efface bits 4-7
```

> `BICS Rdn, Rm` est equivalent a `MVNS Rtmp, Rm` puis `ANDS Rdn, Rtmp`, mais en une seule instruction.

---

## 6.3 Operations de Decalage

### Decalage logique a gauche (LSL)

```
Avant : [b31 b30 ... b1 b0]
LSL #3: [b28 b27 ... b0  0  0  0]
         ↑                      ↑
    bits perdus           zeros inseres

C ← dernier bit sorti
```

```asm
lsls r0, r0, #3      // r0 = r0 << 3 = r0 × 8
lsls r0, r1           // r0 = r0 << r1 (decalage variable)
```

> **LSL de N bits = multiplication par 2^N** (pour les valeurs non signees)

### Decalage logique a droite (LSR)

```
Avant : [b31 b30 ... b1 b0]
LSR #3: [ 0   0   0  b31 b30 ... b3]
          ↑               bits perdus →
     zeros inseres

C ← dernier bit sorti
```

```asm
lsrs r0, r0, #4      // r0 = r0 >> 4 (division non signee par 16)
```

> **LSR de N bits = division entiere non signee par 2^N**

### Decalage arithmetique a droite (ASR)

```
Avant : [1 b30 ... b1 b0]     (nombre negatif, bit 31 = 1)
ASR #3: [1  1   1   1 b30 ... b3]
         ↑
    bit de signe replique (preservé !)
```

```asm
asrs r0, r0, #2      // r0 = r0 >> 2 (division signee par 4)
```

> **ASR preserve le signe** : ideal pour diviser un nombre signe par une puissance de 2.

### Rotation a droite (ROR)

```
Avant : [b31 ... b2 b1 b0]
ROR #3: [b2 b1 b0 b31 ... b3]
          ↑
    bits sortis a droite reviennent a gauche
```

```asm
rors r0, r1           // r0 = rotation droite de r0 par r1 bits
```

### Tableau recapitulatif des decalages

| Instruction | Operation | Equivalent math | Signe |
|------------|-----------|-----------------|-------|
| `LSLS Rd, Rm, #imm5` | Decalage gauche | × 2^N | Non signe |
| `LSRS Rd, Rm, #imm5` | Decalage droite logique | ÷ 2^N | Non signe |
| `ASRS Rd, Rm, #imm5` | Decalage droite arithmetique | ÷ 2^N | **Signe** |
| `RORS Rdn, Rm` | Rotation droite | - | - |

---

## 6.4 Extraction et Insertion de Champs de Bits

### Extraction d'un champ

Pour extraire les bits [7:4] d'un registre :

```asm
// Methode : decaler a droite puis masquer
lsrs r0, r0, #4      // decaler les bits 7:4 vers 3:0
movs r1, #0x0F        // masque pour 4 bits
ands r0, r1           // garder seulement les 4 bits bas
```

### Insertion d'un champ

Pour inserer une valeur dans les bits [7:4] :

```asm
// 1. Effacer le champ cible
movs r2, #0xF0
bics r0, r2           // effacer bits 7:4 de r0

// 2. Positionner la nouvelle valeur
lsls r1, r1, #4      // decaler la valeur vers bits 7:4

// 3. Inserer
orrs r0, r1           // combiner
```

---

## 6.5 Instructions d'Inversion d'Octets

| Instruction | Operation | Utilisation |
|------------|-----------|-------------|
| `REV Rd, Rm` | Inverse les 4 octets | Conversion endianness 32 bits |
| `REV16 Rd, Rm` | Inverse les octets dans chaque demi-mot | Conversion endianness 16 bits |
| `REVSH Rd, Rm` | Inverse les 2 octets bas + extension de signe | Conversion 16 bits signe |

### Exemple REV

```
Avant :  [0x12] [0x34] [0x56] [0x78]   = 0x12345678
REV :    [0x78] [0x56] [0x34] [0x12]   = 0x78563412
```

### Exemple REV16

```
Avant :  [0x12] [0x34] [0x56] [0x78]   = 0x12345678
REV16 :  [0x34] [0x12] [0x78] [0x56]   = 0x34127856
```

---

## Resume

- **AND** : effacer des bits (masque avec 0)
- **OR** : activer des bits (masque avec 1)
- **XOR** : inverser des bits (masque avec 1)
- **BIC** : effacer des bits (masque avec 1, plus direct que AND+NOT)
- **TST** : tester des bits sans modifier
- **LSL** : multiplication par 2^N
- **LSR** : division non signee par 2^N
- **ASR** : division signee par 2^N (preserve le signe)
- **ROR** : rotation
- **REV/REV16/REVSH** : inversion d'octets (endianness)

---

[<< Chapitre 5](./05-Operations-Arithmetiques.md) | [Chapitre 7 : Acces Memoire >>](./07-Acces-Memoire.md)
