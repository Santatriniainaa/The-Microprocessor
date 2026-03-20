# P2 - Calculatrice Scientifique Embarquee ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Difficulte** : Intermediaire | **Chapitres** : 5-8
> **Simulation** : Keil MDK Cortex-M0 Simulator

---

## 1. Objectif Business

Developper un **moteur de calcul embarque** capable d'evaluer des expressions arithmetiques entieres sur 32 bits, communiquant via **UART** (terminal serie). Ce type de composant est utilise dans :
- **Instrumentation de test** : oscilloscopes, multimetres
- **Automates industriels** : calculs de regulation PID
- **Systemes embarques** : evaluation de formules configurables

## 2. Fonctionnalites

- Evaluation d'expressions : `(12 + 34) * 5 - 20 / 4`
- Operations : `+`, `-`, `*`, `/`, `%`, parentheses `()`
- Priorite des operateurs respectee
- Nombres signes 32 bits (-2^31 a 2^31-1)
- Detection d'erreurs (division par zero, debordement, syntaxe)
- Interface UART : saisie expression → affichage resultat

## 3. Architecture

```
UART RX ──→ [Input Buffer] ──→ [Tokenizer] ──→ [Parser]
                                                   │
                                            [Eval Stack]
                                                   │
                                            [Resultat]
                                                   │
                                            [Formatter] ──→ UART TX
```

## 4. Concepts du cours appliques

| Concept | Application |
|---------|------------|D
| Operations arithmetiques (Ch.5) | ADD, SUB, MUL, division logicielle |
| Operations bit (Ch.6) | Masquage, detection signe, conversion |
| Acces memoire (Ch.7) | Pile d'evaluation, buffers |
| Branch / Boucles (Ch.8) | Parser, machine a etats, boucle principale |

---

## 5. Code Source Complet

### 5.1 `calc.h` - Interface principale

```c
/*******************************************************************************
 * @file    calc.h
 * @brief   Calculatrice scientifique embarquee - ARM Cortex-M0
 * @author  Santatriniaina RAZAFINDRAKOTO
 * @date    20 mars 2026
 ******************************************************************************/
#ifndef CALC_H
#define CALC_H

#include <stdint.h>

/*--- Codes d'erreur ---*/
typedef enum {
    CALC_OK              = 0,
    CALC_ERR_SYNTAX      = 1,
    CALC_ERR_DIV_ZERO    = 2,
    CALC_ERR_OVERFLOW     = 3,
    CALC_ERR_PAREN       = 4,
    CALC_ERR_BUFFER_FULL = 5,
    CALC_ERR_STACK_FULL  = 6
} calc_error_t;

/*--- Types de tokens ---*/
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_END
} token_type_t;

typedef struct {
    token_type_t type;
    int32_t      value;   /* valide seulement si type == TOKEN_NUMBER */
} token_t;

/*--- API publique ---*/
calc_error_t calc_evaluate(const char *expression, int32_t *result);
void         calc_error_str(calc_error_t err, char *buf, uint32_t size);

#endif /* CALC_H */
```

### 5.2 `calc.c` - Moteur de calcul (Recursive Descent Parser)

```c
/*******************************************************************************
 * @file    calc.c
 * @brief   Evaluateur d'expressions arithmetiques
 * @details Utilise un parser recursive descent avec priorite d'operateurs :
 *          Niveau 1 (bas)  : + -
 *          Niveau 2 (haut) : * / %
 *          Niveau 3        : ( ) et nombres
 ******************************************************************************/
#include "calc.h"

/*--- Variables internes du parser ---*/
static const char *g_expr;      /* pointeur courant dans l'expression */
static token_t     g_current;   /* token courant */
static calc_error_t g_error;    /* derniere erreur */

/*==========================================================================
 * TOKENIZER : decoupe l'expression en tokens
 *=========================================================================*/

static void skip_spaces(void) {
    while (*g_expr == ' ' || *g_expr == '\t') {
        g_expr++;
    }
}

static int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static void next_token(void) {
    skip_spaces();

    if (*g_expr == '\0' || *g_expr == '\r' || *g_expr == '\n') {
        g_current.type = TOKEN_END;
        return;
    }

    /* Nombre */
    if (is_digit(*g_expr)) {
        int32_t val = 0;
        while (is_digit(*g_expr)) {
            int32_t digit = (*g_expr - '0');
            /* Detection debordement */
            if (val > (0x7FFFFFFF - digit) / 10) {
                g_error = CALC_ERR_OVERFLOW;
                g_current.type = TOKEN_END;
                return;
            }
            val = val * 10 + digit;
            g_expr++;
        }
        g_current.type  = TOKEN_NUMBER;
        g_current.value = val;
        return;
    }

    /* Operateurs et parentheses */
    char c = *g_expr++;
    switch (c) {
        case '+': g_current.type = TOKEN_PLUS;   break;
        case '-': g_current.type = TOKEN_MINUS;  break;
        case '*': g_current.type = TOKEN_MUL;    break;
        case '/': g_current.type = TOKEN_DIV;    break;
        case '%': g_current.type = TOKEN_MOD;    break;
        case '(': g_current.type = TOKEN_LPAREN; break;
        case ')': g_current.type = TOKEN_RPAREN; break;
        default:
            g_error = CALC_ERR_SYNTAX;
            g_current.type = TOKEN_END;
            break;
    }
}

/*==========================================================================
 * PARSER : grammaire recursive descent
 *   expression = term (('+' | '-') term)*
 *   term       = factor (('*' | '/' | '%') factor)*
 *   factor     = NUMBER | '(' expression ')' | '-' factor
 *=========================================================================*/

static int32_t parse_expression(void);

static int32_t parse_factor(void) {
    if (g_error != CALC_OK) return 0;

    /* Nombre negatif unaire */
    if (g_current.type == TOKEN_MINUS) {
        next_token();
        return -parse_factor();
    }

    /* Parentheses */
    if (g_current.type == TOKEN_LPAREN) {
        next_token();
        int32_t val = parse_expression();
        if (g_current.type != TOKEN_RPAREN) {
            g_error = CALC_ERR_PAREN;
            return 0;
        }
        next_token();
        return val;
    }

    /* Nombre */
    if (g_current.type == TOKEN_NUMBER) {
        int32_t val = g_current.value;
        next_token();
        return val;
    }

    g_error = CALC_ERR_SYNTAX;
    return 0;
}

static int32_t parse_term(void) {
    int32_t left = parse_factor();

    while (g_error == CALC_OK &&
           (g_current.type == TOKEN_MUL ||
            g_current.type == TOKEN_DIV ||
            g_current.type == TOKEN_MOD)) {

        token_type_t op = g_current.type;
        next_token();
        int32_t right = parse_factor();

        if (g_error != CALC_OK) return 0;

        switch (op) {
            case TOKEN_MUL:
                left = left * right;
                break;
            case TOKEN_DIV:
                if (right == 0) { g_error = CALC_ERR_DIV_ZERO; return 0; }
                left = left / right;
                break;
            case TOKEN_MOD:
                if (right == 0) { g_error = CALC_ERR_DIV_ZERO; return 0; }
                left = left % right;
                break;
            default: break;
        }
    }
    return left;
}

static int32_t parse_expression(void) {
    int32_t left = parse_term();

    while (g_error == CALC_OK &&
           (g_current.type == TOKEN_PLUS ||
            g_current.type == TOKEN_MINUS)) {

        token_type_t op = g_current.type;
        next_token();
        int32_t right = parse_term();

        if (g_error != CALC_OK) return 0;

        if (op == TOKEN_PLUS)  left = left + right;
        else                   left = left - right;
    }
    return left;
}

/*==========================================================================
 * API PUBLIQUE
 *=========================================================================*/

calc_error_t calc_evaluate(const char *expression, int32_t *result) {
    g_expr  = expression;
    g_error = CALC_OK;

    next_token();
    *result = parse_expression();

    if (g_error == CALC_OK && g_current.type != TOKEN_END) {
        g_error = CALC_ERR_SYNTAX;
    }
    return g_error;
}

void calc_error_str(calc_error_t err, char *buf, uint32_t size) {
    static const char *msgs[] = {
        "OK",
        "Erreur de syntaxe",
        "Division par zero",
        "Debordement",
        "Parenthese manquante",
        "Buffer plein",
        "Pile pleine"
    };
    uint32_t i = 0;
    const char *src = (err <= CALC_ERR_STACK_FULL) ? msgs[err] : "Inconnue";
    while (*src && i < size - 1) { buf[i++] = *src++; }
    buf[i] = '\0';
}
```

### 5.3 `uart_io.h` / `uart_io.c` - Interface serie

```c
/*--- uart_io.h ---*/
#ifndef UART_IO_H
#define UART_IO_H

#include <stdint.h>

void     uart_init(uint32_t baud_rate);
void     uart_send_char(char c);
void     uart_send_string(const char *str);
void     uart_send_int(int32_t val);
char     uart_recv_char(void);
uint32_t uart_recv_line(char *buf, uint32_t max_len);

#endif
```

```c
/*--- uart_io.c ---*/
#include "uart_io.h"

#define UART0_DR   (*(volatile uint32_t *)0x40004000)
#define UART0_FR   (*(volatile uint32_t *)0x40004018)
#define UART0_IBRD (*(volatile uint32_t *)0x40004024)
#define UART0_FBRD (*(volatile uint32_t *)0x40004028)
#define UART0_LCR  (*(volatile uint32_t *)0x4000402C)
#define UART0_CR   (*(volatile uint32_t *)0x40004030)

#define FR_TXFF    (1 << 5)   /* TX FIFO full */
#define FR_RXFE    (1 << 4)   /* RX FIFO empty */

void uart_init(uint32_t baud_rate) {
    /* Configuration simplifiee pour simulateur Keil */
    UART0_CR   = 0;                    /* desactiver UART */
    UART0_IBRD = 12000000 / (16 * baud_rate);
    UART0_FBRD = 0;
    UART0_LCR  = (3 << 5) | (1 << 4); /* 8 bits, FIFO enable */
    UART0_CR   = (1 << 0) | (1 << 8) | (1 << 9); /* UART, TX, RX enable */
}

void uart_send_char(char c) {
    while (UART0_FR & FR_TXFF);  /* attendre que TX soit libre */
    UART0_DR = c;
}

void uart_send_string(const char *str) {
    while (*str) {
        if (*str == '\n') uart_send_char('\r');
        uart_send_char(*str++);
    }
}

void uart_send_int(int32_t val) {
    char buf[12];
    int i = 0;
    uint32_t uval;

    if (val < 0) {
        uart_send_char('-');
        uval = (uint32_t)(-(val + 1)) + 1;  /* eviter overflow -INT_MIN */
    } else {
        uval = (uint32_t)val;
    }

    if (uval == 0) { uart_send_char('0'); return; }

    while (uval > 0) {
        buf[i++] = '0' + (uval % 10);
        uval /= 10;
    }
    while (i > 0) {
        uart_send_char(buf[--i]);
    }
}

char uart_recv_char(void) {
    while (UART0_FR & FR_RXFE);  /* attendre donnee */
    return (char)(UART0_DR & 0xFF);
}

uint32_t uart_recv_line(char *buf, uint32_t max_len) {
    uint32_t i = 0;
    while (i < max_len - 1) {
        char c = uart_recv_char();
        if (c == '\r' || c == '\n') {
            uart_send_string("\r\n");
            break;
        }
        if (c == '\b' || c == 0x7F) {  /* backspace */
            if (i > 0) {
                i--;
                uart_send_string("\b \b");
            }
            continue;
        }
        buf[i++] = c;
        uart_send_char(c);  /* echo */
    }
    buf[i] = '\0';
    return i;
}
```

### 5.4 `main.c` - Programme principal

```c
/*******************************************************************************
 * @file    main.c
 * @brief   Calculatrice scientifique - Boucle principale REPL
 ******************************************************************************/
#include "calc.h"
#include "uart_io.h"

#define INPUT_BUF_SIZE  80

static void print_banner(void) {
    uart_send_string("\r\n");
    uart_send_string("==============================================\r\n");
    uart_send_string("  Calculatrice ARM Cortex-M0 v1.0\r\n");
    uart_send_string("  Auteur: S. RAZAFINDRAKOTO | 2026\r\n");
    uart_send_string("  Operations: + - * / % ()\r\n");
    uart_send_string("  Tapez 'q' pour quitter\r\n");
    uart_send_string("==============================================\r\n");
}

int main(void) {
    char input[INPUT_BUF_SIZE];
    int32_t result;
    calc_error_t err;
    char err_msg[32];

    uart_init(115200);
    print_banner();

    /* Boucle REPL : Read-Eval-Print-Loop */
    while (1) {
        uart_send_string("\r\n> ");
        uint32_t len = uart_recv_line(input, INPUT_BUF_SIZE);

        if (len == 0) continue;
        if (input[0] == 'q' || input[0] == 'Q') {
            uart_send_string("Au revoir.\r\n");
            break;
        }

        err = calc_evaluate(input, &result);

        if (err == CALC_OK) {
            uart_send_string("= ");
            uart_send_int(result);
            uart_send_string("\r\n");
        } else {
            uart_send_string("ERREUR: ");
            calc_error_str(err, err_msg, sizeof(err_msg));
            uart_send_string(err_msg);
            uart_send_string("\r\n");
        }
    }

    while (1);
}
```

### 5.5 Tests de validation

| Expression | Resultat attendu | Test |
|-----------|-----------------|------|
| `2 + 3` | 5 | Addition simple |
| `10 - 25` | -15 | Resultat negatif |
| `6 * 7` | 42 | Multiplication |
| `100 / 3` | 33 | Division entiere |
| `17 % 5` | 2 | Modulo |
| `(2 + 3) * 4` | 20 | Parentheses |
| `2 + 3 * 4` | 14 | Priorite operateurs |
| `((10 + 5) * 2) - 3` | 27 | Parentheses imbriquees |
| `-5 + 3` | -2 | Negatif unaire |
| `10 / 0` | ERREUR: Division par zero | Gestion d'erreur |
| `2 + + 3` | ERREUR: Syntaxe | Gestion d'erreur |
| `(2 + 3` | ERREUR: Parenthese manquante | Gestion d'erreur |

---

[Retour a l'index](../00-INDEX-PROJET.md)
