#pragma once
#ifndef SECDED_H
#define SECDED_H

#include <stdio.h>
#include <stdbool.h>
#define LOG(f, ...) printf("secded: " f "\n", ##__VA_ARGS__)

#ifdef ENABLE_TRACE
#define LOG_TRACE(...) LOG(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#define TRACEX(v) LOG_TRACE(#v " == 0x%x", v)
#define TRACEU(v) LOG_TRACE(#v " == %u", v)

/*
 SECDED goes for Single Error Correct Double Error Detect
 code is calculated from data organized as a 3x3 array of bits
 for a data 0b0000ABCDEFGHIJK 
 and a code 0b0 c6 c5c4c3 c2c1c0
 =>
   0  1  2
 0 A  B  C  c5
 1 E  F  G  c4
 2 I  J  K  c3
   c2 c1 c0 c6
 with c0 = parity(C, G, K)
      c3 = parity(I, J, K)
      c6 = parity(c0, c1, c2) = parity(c3, c4, c5)
*/

#include <stdint.h>

/* x == 0 => MSB(x) = 0
   x != 0 => MSB(x) = floor(log2(x)) + 1
 */
static inline unsigned MSB(unsigned b) {
    unsigned n = 0;
    while (b) {
        b &= ~(1<<n++);
    }

    return n;
} 

#define SD_MAT_SIDE_SZ 3

#if SD_MAT_SIDE_SZ == 3
#define SD_ROW_MASK 0x7 // 0b0111
#define SD_COL_MASK 0x49 // 0b0 0100 1001

#define SD_CODE_COL_MASK 0x7     // 0b000 0111
#define SD_CODE_ROW_MASK 0x38    // 0b011 1000
#define SD_CODE_PARITY_MASK 0x40 // 0b100 0000

#define BIT_AT(r, c) (1<<(( (SD_MAT_SIDE_SZ - 1 - (r)) * SD_MAT_SIDE_SZ) \
                          + (SD_MAT_SIDE_SZ - 1 - (c))))
#endif

/* https://stackoverflow.com/a/109025/647828 */
static unsigned bit_count_ones(uint32_t i) {
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static inline uint8_t secded_code(uint16_t data) {
    uint8_t c = 0;
    
    for (unsigned row_col = 0; row_col < SD_MAT_SIDE_SZ; row_col++) {
        unsigned row_sel = data >> (SD_MAT_SIDE_SZ * row_col) & SD_ROW_MASK;
        c |= (bit_count_ones(row_sel) % 2) << (row_col + SD_MAT_SIDE_SZ);
        
        unsigned col_sel = data >> (row_col) & SD_COL_MASK;
        c |= (bit_count_ones(col_sel) % 2) << (row_col);
    }
    
    if (bit_count_ones(c & SD_CODE_ROW_MASK) % 2) {
        c |= SD_CODE_PARITY_MASK;
    }
    
    return c;
}

static inline int secded(uint16_t data, uint16_t *corrected, uint8_t code) {
    uint8_t actual_code = secded_code(data);
    TRACEX(code);
    TRACEX(actual_code);
    uint8_t error_bits = actual_code ^ code;
    
    uint8_t err_rc = error_bits & ~SD_CODE_PARITY_MASK,
            col = code & SD_CODE_COL_MASK,
            err_col = error_bits & SD_CODE_COL_MASK,
            row = code & SD_CODE_ROW_MASK,
            err_row = (error_bits & SD_CODE_ROW_MASK)>>SD_MAT_SIDE_SZ;

    TRACEX(error_bits);
    TRACEX(col);
    TRACEX(row);
    unsigned total_errors = bit_count_ones(err_rc);
    bool parities_agree = bit_count_ones(col)%2 == bit_count_ones(row)%2
                          && (code & SD_CODE_PARITY_MASK) == bit_count_ones(row)%2;

    bool can_correct = total_errors == 0 || (
                       bit_count_ones(err_col) == 1
                        && bit_count_ones(err_row) == 1
                        && parities_agree);
    if (total_errors == 1) {
        /*parity must agree for the one (col or row) that has no error*/
        if (err_col) {
            can_correct = parities_agree;
        } else {
            can_correct = (code & SD_CODE_PARITY_MASK) == bit_count_ones(col)%2;
        }
    }

    if (can_correct) {
        if (total_errors == 2) {
            TRACEU(MSB(err_row));
            TRACEU(MSB(err_col));
            uint16_t bit_correct = BIT_AT(SD_MAT_SIDE_SZ - MSB(err_row),
                                          SD_MAT_SIDE_SZ - MSB(err_col));
            TRACEX(bit_correct);
            data ^= bit_correct;
        }
        *corrected = data;
    }

    return can_correct ? 0 : 1;
}
#endif /*SECDED_H*/
