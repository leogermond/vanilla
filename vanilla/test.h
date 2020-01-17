#ifndef VANILLA_H
#define VANILLA_H

#define VT100_RED "\033[31m"
#define VT100_YLW "\033[33m"
#define VT100_RST "\033[0m"

#define TEST(name, body) do { \
    const char *current_test_name = name; \
    printf(" test %s\r\n", current_test_name); \
    int rtest = 0; \
    body \
    r |= rtest; \
} while(0)

#define skip() printf(VT100_YLW "  S   %s" VT100_RST "\r\n", \
                      current_test_name); break

#define ASSERT_OP_GEN(f, as, a, op, bs, b) do { \
    unsigned a_safe_ = a, b_safe_ = b; \
    bool safe_assert_ = ((a_safe_) op (b_safe_));\
    if (! safe_assert_) { \
        rtest = 1; \
        printf(VT100_RED \
              "  F   %s  %s:%d assert " as " (" f ") " #op " " bs " (" f ")\r\n" \
              VT100_RST, current_test_name, \
              __FILE__, __LINE__, a_safe_, b_safe_); \
    } \
} while(0)

#define ASSERT_EQ_GEN(f, as, a, bs, b) ASSERT_OP_GEN(f, as, a, ==, bs, b)
#define ASSERT_NE_GEN(f, as, a, bs, b) ASSERT_OP_GEN(f, as, a, !=, bs, b)

#define ASSERT_UINT_EQ(a, b) ASSERT_EQ_GEN("%u", #a, a, #b, b)
#define ASSERT_X_EQ(a, b) ASSERT_EQ_GEN("0x%x", #a, a, #b, b)
#define ASSERT_UINT_NE(a, b) ASSERT_NE_GEN("%u", #a, a, #b, b)
#define ASSERT_X_NE(a, b) ASSERT_NE_GEN("0x%x", #a, a, #b, b)

#endif /*VANILLA_H*/
