#ifndef _BITOPS_H
/* ============== */

#define _BITOPS_H 1

/* Type representing a bit's position */
typedef unsigned char bitpos_t;
/* Type representing the number of bits used for padding. */
typedef unsigned char bitpadding_t;
/* Boolean type */
typedef unsigned char bool_t; 

/* OR bit b and the i'th bit of n. */
#define BIT_OR(n, i, b) \
	((n) |= (b) << (i))

/* Set the i'th bit of n to b. */
#define BIT_SET(n, i, b) \
	((n) ^= (-(b) ^ (n)) & (1U << (i)))

/* Check whether the i'th bit of n is high. */
#define BIT_CHECK(n, i) \
	(((n) >> (i)) & 1U)

/* Set the i'th bit of n to low. */
#define BIT_CLEAR(n, i) \
	((n) &= ~(1U << (i)))

/* ============== */
#endif