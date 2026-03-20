# P4 - Protocole de Communication Fiable ARM Cortex-M0

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026
> **Difficulte** : Avance | **Chapitres** : 6, 8, 19
> **Simulation** : Keil MDK Cortex-M0 Simulator

---

## 1. Objectif Business

Implementer un **protocole de communication serie fiable** au-dessus de l'UART, avec :
- **Trame structuree** : start, longueur, donnees, CRC, stop
- **Detection d'erreurs** : CRC-8 polynomial
- **Acquittement** : ACK/NACK avec retransmission
- **Machine a etats** : reception robuste avec timeout

**Secteurs** : automobile ($180B embedded), industriel (bus de terrain), medical (dispositifs connectes).

## 2. Format de Trame

```
+-------+------+------+--------+--------+------+-------+
| START | SEQ  | LEN  | DATA   |  DATA  | CRC8 |  END  |
| 0x7E  | 1B   | 1B   | 0..64B | ...    | 1B   | 0x7F  |
+-------+------+------+--------+--------+------+-------+
  1 octet 1 oct  1 oct  0-64 oct          1 oct  1 octet

START  = 0x7E (delimiteur debut)
SEQ    = Numero de sequence (0-255, rotation)
LEN    = Longueur du champ DATA (0-64)
DATA   = Donnees utiles
CRC8   = CRC-8 sur SEQ + LEN + DATA
END    = 0x7F (delimiteur fin)
```

## 3. Architecture

```
              Emetteur                          Recepteur
         ┌──────────────┐                 ┌──────────────┐
         │  Application │                 │  Application │
         │     send()   │                 │    recv()    │
         └──────┬───────┘                 └──────▲───────┘
                │                                │
         ┌──────▼───────┐                 ┌──────┴───────┐
         │   Protocol   │    UART TX      │   Protocol   │
         │  - Framing   │───────────────→ │  - Deframing │
         │  - CRC calc  │                 │  - CRC check │
         │  - Wait ACK  │◀─── UART RX ───│  - Send ACK  │
         │  - Retry     │                 │  - Seq check │
         └──────────────┘                 └──────────────┘
```

## 4. Code Source Complet

### 4.1 `protocol.h`

```c
/*******************************************************************************
 * @file    protocol.h
 * @brief   Protocole de communication fiable sur UART
 ******************************************************************************/
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PROTO_START        0x7E
#define PROTO_END          0x7F
#define PROTO_MAX_DATA     64
#define PROTO_MAX_RETRIES  3
#define PROTO_TIMEOUT_MS   500
#define PROTO_ACK          0x06
#define PROTO_NACK         0x15

typedef enum {
    PROTO_OK            = 0,
    PROTO_ERR_CRC       = 1,
    PROTO_ERR_TIMEOUT   = 2,
    PROTO_ERR_SEQUENCE  = 3,
    PROTO_ERR_OVERFLOW  = 4,
    PROTO_ERR_FORMAT    = 5,
    PROTO_ERR_MAX_RETRY = 6
} proto_status_t;

typedef struct {
    uint8_t  seq;                       /* numero de sequence */
    uint8_t  len;                       /* longueur data */
    uint8_t  data[PROTO_MAX_DATA];      /* donnees */
    uint8_t  crc;                       /* CRC-8 calcule */
} proto_frame_t;

typedef struct {
    uint8_t  tx_seq;            /* prochain numero de sequence TX */
    uint8_t  rx_seq_expected;   /* prochain numero de sequence RX attendu */
    uint32_t tx_count;          /* compteur de trames envoyees */
    uint32_t rx_count;          /* compteur de trames reçues */
    uint32_t err_count;         /* compteur d'erreurs */
    uint32_t retry_count;       /* compteur de retransmissions */
} proto_stats_t;

/*--- API ---*/
void           proto_init(proto_stats_t *stats);
uint8_t        proto_crc8(const uint8_t *data, uint32_t len);
proto_status_t proto_send(proto_stats_t *stats, const uint8_t *data,
                          uint32_t len);
proto_status_t proto_receive(proto_stats_t *stats, uint8_t *data,
                             uint32_t *len);
void           proto_print_stats(const proto_stats_t *stats);

#endif
```

### 4.2 `crc8.c` - Calcul CRC-8

```c
/*******************************************************************************
 * @file    crc8.c
 * @brief   CRC-8 polynomial : x^8 + x^2 + x + 1 (0x07)
 * @details Utilise les operations bit a bit du Chapitre 6
 ******************************************************************************/
#include "protocol.h"

/* Table CRC-8 pre-calculee (polynomial 0x07) */
static const uint8_t crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

uint8_t proto_crc8(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0x00;
    for (uint32_t i = 0; i < len; i++) {
        crc = crc8_table[crc ^ data[i]];
    }
    return crc;
}
```

### 4.3 `protocol.c` - Logique du protocole

```c
/*******************************************************************************
 * @file    protocol.c
 * @brief   Implementation du protocole fiable
 ******************************************************************************/
#include "protocol.h"
#include "uart_io.h"

void proto_init(proto_stats_t *stats) {
    stats->tx_seq          = 0;
    stats->rx_seq_expected = 0;
    stats->tx_count        = 0;
    stats->rx_count        = 0;
    stats->err_count       = 0;
    stats->retry_count     = 0;
}

/*--- Envoi d'une trame avec retransmission ---*/
proto_status_t proto_send(proto_stats_t *stats, const uint8_t *data,
                          uint32_t len) {
    if (len > PROTO_MAX_DATA) return PROTO_ERR_OVERFLOW;

    uint8_t frame_buf[PROTO_MAX_DATA + 2]; /* seq + len + data */
    frame_buf[0] = stats->tx_seq;
    frame_buf[1] = (uint8_t)len;
    for (uint32_t i = 0; i < len; i++) {
        frame_buf[2 + i] = data[i];
    }
    uint8_t crc = proto_crc8(frame_buf, len + 2);

    for (uint32_t attempt = 0; attempt <= PROTO_MAX_RETRIES; attempt++) {
        /* Envoyer la trame */
        uart_send_char(PROTO_START);
        uart_send_char(stats->tx_seq);
        uart_send_char((uint8_t)len);
        for (uint32_t i = 0; i < len; i++) {
            uart_send_char(data[i]);
        }
        uart_send_char(crc);
        uart_send_char(PROTO_END);
        stats->tx_count++;

        /* Attendre ACK/NACK (simplifie pour simulation) */
        char response = uart_recv_char();
        if (response == PROTO_ACK) {
            stats->tx_seq++;
            return PROTO_OK;
        }

        /* NACK ou timeout → retransmission */
        stats->retry_count++;
    }

    stats->err_count++;
    return PROTO_ERR_MAX_RETRY;
}

/*--- Reception d'une trame ---*/
proto_status_t proto_receive(proto_stats_t *stats, uint8_t *data,
                             uint32_t *len) {
    /* Attendre START */
    char c = uart_recv_char();
    if (c != PROTO_START) return PROTO_ERR_FORMAT;

    /* Lire SEQ */
    uint8_t seq = (uint8_t)uart_recv_char();

    /* Lire LEN */
    uint8_t data_len = (uint8_t)uart_recv_char();
    if (data_len > PROTO_MAX_DATA) {
        stats->err_count++;
        uart_send_char(PROTO_NACK);
        return PROTO_ERR_OVERFLOW;
    }

    /* Lire DATA */
    uint8_t frame_buf[PROTO_MAX_DATA + 2];
    frame_buf[0] = seq;
    frame_buf[1] = data_len;
    for (uint32_t i = 0; i < data_len; i++) {
        data[i] = (uint8_t)uart_recv_char();
        frame_buf[2 + i] = data[i];
    }
    *len = data_len;

    /* Lire CRC */
    uint8_t rx_crc = (uint8_t)uart_recv_char();

    /* Lire END */
    c = uart_recv_char();
    if (c != PROTO_END) {
        stats->err_count++;
        uart_send_char(PROTO_NACK);
        return PROTO_ERR_FORMAT;
    }

    /* Verifier CRC */
    uint8_t calc_crc = proto_crc8(frame_buf, data_len + 2);
    if (rx_crc != calc_crc) {
        stats->err_count++;
        uart_send_char(PROTO_NACK);
        return PROTO_ERR_CRC;
    }

    /* Verifier sequence */
    if (seq != stats->rx_seq_expected) {
        stats->err_count++;
        uart_send_char(PROTO_NACK);
        return PROTO_ERR_SEQUENCE;
    }

    /* Tout OK → ACK */
    stats->rx_seq_expected++;
    stats->rx_count++;
    uart_send_char(PROTO_ACK);
    return PROTO_OK;
}

void proto_print_stats(const proto_stats_t *stats) {
    uart_send_string("=== Statistiques Protocole ===\r\n");
    uart_send_string("TX: "); uart_send_int(stats->tx_count);
    uart_send_string("  RX: "); uart_send_int(stats->rx_count);
    uart_send_string("  ERR: "); uart_send_int(stats->err_count);
    uart_send_string("  RETRY: "); uart_send_int(stats->retry_count);
    uart_send_string("\r\n");
}
```

### 4.4 `main.c`

```c
#include "protocol.h"
#include "uart_io.h"

int main(void) {
    proto_stats_t stats;
    uint8_t test_data[] = "Hello Cortex-M0!";

    uart_init(115200);
    proto_init(&stats);

    uart_send_string("=== Protocole Fiable v1.0 ===\r\n");

    /* Test : envoyer 10 messages */
    for (uint32_t i = 0; i < 10; i++) {
        test_data[0] = '0' + (i % 10);
        proto_status_t st = proto_send(&stats, test_data, 16);
        if (st != PROTO_OK) {
            uart_send_string("ERREUR envoi #");
            uart_send_int(i);
            uart_send_string("\r\n");
        }
    }

    proto_print_stats(&stats);

    while (1);
}
```

---

[Retour a l'index](../00-INDEX-PROJET.md)
