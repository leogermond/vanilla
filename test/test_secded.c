/*
Copyright (C) 2020  Léo Germond

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "secded.h"

#define TEST(name, body) do { \
    const char *current_test_name = name; \
    info("test %s", current_test_name); \
    int rtest = 0; \
    body \
    r |= rtest; \
    break; \
} while(0)

#define skip() warning("  S   %s", current_test_name); break

#define ASSERT_OP_GEN(f, as, a, op, bs, b) do { \
    unsigned a_safe_ = a, b_safe_ = b; \
    bool safe_assert_ = ((a_safe_) op (b_safe_));\
    if (! safe_assert_) { \
        rtest = 1; \
        err("  F   %s  %s:%d assert " as " (" f ") " #op " " bs " (" f ")", \
            current_test_name, __FILE__, __LINE__, a_safe_, b_safe_); \
    } \
} while(0)

#define ASSERT_EQ_GEN(f, as, a, bs, b) ASSERT_OP_GEN(f, as, a, ==, bs, b)
#define ASSERT_NE_GEN(f, as, a, bs, b) ASSERT_OP_GEN(f, as, a, !=, bs, b)

#define ASSERT_UINT_EQ(a, b) ASSERT_EQ_GEN("%u", #a, a, #b, b)
#define ASSERT_X_EQ(a, b) ASSERT_EQ_GEN("0x%x", #a, a, #b, b)
#define ASSERT_UINT_NE(a, b) ASSERT_NE_GEN("%u", #a, a, #b, b)
#define ASSERT_X_NE(a, b) ASSERT_NE_GEN("0x%x", #a, a, #b, b)

int test_secded() {
    printf("test suite SECDED\r\n");
    int r = 0;

    TEST("bit counting", {
        ASSERT_UINT_EQ(3, bit_count_ones(0x7));
        ASSERT_UINT_EQ(3, bit_count_ones(0x7<<1));
        ASSERT_UINT_EQ(3, bit_count_ones(0x7<<29));
    });

    TEST("MSB", {
        ASSERT_UINT_EQ(0, MSB(0));
        ASSERT_UINT_EQ(1, MSB(0x1));
        ASSERT_UINT_EQ(4, MSB(0x8));
        ASSERT_UINT_EQ(30, MSB(1<<29));
    });

    TEST("BIT_AT", {
        ASSERT_X_EQ(0x100, BIT_AT(0, 0));
        ASSERT_X_EQ(0x040, BIT_AT(0, 2));
        ASSERT_X_EQ(0x001, BIT_AT(2, 2));
    });
    
    TEST("secded_code row", {
        /* 1 0 0 1
           1 0 1 0
           0 0 1 1
           0 0 0 0
         */
        ASSERT_UINT_EQ(0x28, secded_code(0x129));
    });
    
    TEST("secded_code col", {
        /* 1 1 0 0
           0 0 0 0
           0 1 1 0
           1 0 1 0
         */
        ASSERT_UINT_EQ(0x5, secded_code(0x183));
    });
    
    TEST("secded_code row + col", {
        /* 1 0 0 1
           0 0 0 0
           0 0 1 1
           1 0 1 0
         */
        ASSERT_UINT_EQ(0x2d, secded_code(0x101));
    });
    
    TEST("secded_code parity 1", {
        /* 1 0 0 1
           0 0 0 0
           0 0 0 0
           1 0 0 1
         */
        ASSERT_UINT_EQ(0x64, secded_code(0x100));
    });
    
    TEST("secded_code col row encoding order", {
        /* 0 0 0 0
           0 0 0 0
           1 0 0 1
           1 0 0 1
         */
        ASSERT_UINT_EQ(0x4c, secded_code(0x004));
    });
    
    TEST("secded_code 0x14", {
        /* 0 0 0 0
           0 1 0 1
           1 0 0 1
           1 1 0 0
         */
        ASSERT_UINT_EQ(0x1e, secded_code(0x014));
    });

    TEST("secded no error", {
        /* 1 0 0 1
           0 0 0 0
           0 0 1 1
           1 0 1 0
           received code = 0b0101101 OK
         */
        uint16_t correct = 0;
        ASSERT_UINT_EQ(0, secded(0x101, &correct, 0x2d));
        ASSERT_UINT_EQ(0x101, correct);
    });

    TEST("secded single correct (data)", {
        /* 1 0 (1) 1
           0 0  0  0
           0 0  1  1
           1 0  1  0
         */
        uint16_t correct = 0;
        ASSERT_UINT_EQ(0, secded(0x141, &correct, 0x2d));
        ASSERT_UINT_EQ(0x101, correct);
    });

    TEST("secded single correct (code)", {
        /* 1 0 0 (0)
           0 0 0  0
           0 0 1  1
           1 0 1  0
         */
        uint16_t correct = 0;
        ASSERT_UINT_EQ(0, secded(0x101, &correct, 0x0d));
        ASSERT_UINT_EQ(0x101, correct);
    });

    TEST("secded single correct (parity)", {
        /* 1 0 0  1
           0 0 0  0
           0 0 1  1
           1 0 1 (1)
         */
        uint16_t correct = 0;
        ASSERT_UINT_EQ(0, secded(0x101, &correct, 0x6d));
        ASSERT_UINT_EQ(0x101, correct);
    });

    TEST("secded double detect (data + data)", {
        /* 1  0 (1) 1
           0 (1) 0  0
           0  0  1  1
           1  0  1  0
         */
        uint16_t correct = 0;
        ASSERT_UINT_NE(0, secded(0x1a1, &correct, 0x2d));
    });

    TEST("secded double detect (code + code)", {
        /* 1  0  0 (0)
           0  0  0  0
           0  0  1  1
           1  0 (0) 0
         */
        uint16_t correct = 0;
        ASSERT_UINT_NE(0, secded(0x101, &correct, 0x0c));
    });

    TEST("secded double detect (code + code only col)", {
        /*(0)(1) 0  0
           0  1  0  1
           1  0  1  1
           1  0  1  0
         */
        uint16_t correct = 0;
        ASSERT_UINT_NE(0, secded(0x154, &correct, 0x1d));
    });

    TEST("secded double detect (code + parity)", {
        /* 1  0  0 (0)
           0  0  0  0
           0  0  1  1
           1  0  1 (1)
         */
        uint16_t correct = 0;
        ASSERT_UINT_NE(0, secded(0x101, &correct, 0x4d));
    });

    TEST("secded double detect (code + data)", {
        /* 1  0 (1) (0)
           0  0  0   0
           0  0  1   1
           1  0  1   0
         */
        uint16_t correct = 0;
        ASSERT_UINT_NE(0, secded(0x141, &correct, 0x0d));
    });

    return r;
}


int main() {
    int r = 0;
    log_verbose = true;
    log_name = "test secded";

    r |= test_secded();

    printf("tests %s\n", r?"FAIL":"SUCCESS");

    return r;
}
