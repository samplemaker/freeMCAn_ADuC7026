/** \file firmware/table-element.h
 * \brief Table Element type definitions
 *
 * \author Copyright (C) 2010 samplemaker
 * \author Copyright (C) 2010 Hans Ulrich Niedermann <hun@n-dimensional.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 * \defgroup table_element Table Element type
 * \ingroup firmware_generic
 * @{
 */

#ifndef TABLE_ELEMENT_H
#define TABLE_ELEMENT_H


#ifndef BITS_PER_VALUE
# error Error: You MUST define BITS_PER_VALUE before #include "table-element.h"!
#endif


/** Unsigned 24bit integer type
 *
 * This could be called a uint24_t, but we do not want to intrude on
 * that namespace.
 */
typedef uint8_t freemcan_uint24_t[3];


/** Histogram element type */
typedef
#if (BITS_PER_VALUE == 8)
  uint8_t
#elif (BITS_PER_VALUE == 12)
  uint16_t
#elif (BITS_PER_VALUE == 16)
  uint16_t
#elif (BITS_PER_VALUE == 24)
  freemcan_uint24_t
#elif (BITS_PER_VALUE == 32)
  uint32_t
#else
# error Unsupported BITS_PER_VALUE
#endif
  table_element_t;


#if (BITS_PER_VALUE == 24)

/** Increment 24bit unsigned integer */

inline static
void table_element_zero(volatile freemcan_uint24_t *dest)
{
 asm("\n\t"

/* \todo */      : /* output operands */

      : /* input operands */
        [preg] "b" (dest)
        /* no clobber */
      );

}

inline static
void table_element_copy(volatile freemcan_uint24_t *dest,
                        volatile freemcan_uint24_t *source)
{
  asm("\n\t"

/* \todo */      : /* output operands */

      : /* input operands */
        [dst] "b" (dest),
        [src] "b" (source)
        /* no clobbers */
      );

}



inline static
void table_element_inc(volatile freemcan_uint24_t *element)
{
  register uint32_t r0, r1;
  asm volatile("\n\t"
               /* load three bytes with respect to endianess (MSB2LSB) */
               "mov   %[r0], #0                     \n\t"
               "ldrb  %[r1], [%[elem], #2]          \n\t"
               "orr   %[r0], %[r1] ,LSL #16         \n\t"
               "ldrb  %[r1], [%[elem], #1]          \n\t"
               "orr   %[r0], %[r1] ,LSL #8          \n\t"
               "ldrb  %[r1], [%[elem]]              \n\t"
               "orr   %[r0], %[r1]                  \n\t"
               /* increase by one */
               "add   %[r0], %[r0], #1              \n\t"
               /* store three bytes with respect to endianess (LSB2MSB) */
               "strb  %[r0], [%[elem]]              \n\t"
               "mov   %[r0], %[r0] ,LSR #8          \n\t"
               "strb  %[r0], [%[elem], #1]          \n\t"
               "mov   %[r0], %[r0] ,LSR #8          \n\t"
               "strb  %[r0], [%[elem], #2]          \n\t"
               : /* output operands */
                 /* let compiler decide which registers to clobber */
                 [r1] "=&r" (r1), [r0] "=&r" (r0),
                 /* input and output operand (treated inside output list) */
                 [elem] "+r" (element)
               : /* no input operands */
                 /* inform that we change the condition code flag */
                 /* : "cc"*/
                 /* store all cached values before and reload them after */
               : "memory"
  );
}


/** Compare table element to value for equality.
 *
 * \param element Pointer to the table element to compare.
 * \param value   The value to compare the element against.
 *
 * Note that #value has a different type from #*element here, as the
 * compiler would not know how to generate or pass such a
 * freemcan_uint24_t value.
 *
 * Caveats: Ignores the upper 8bit of the uint32_t constant. Generates
 * more instructions than strictly necessary. Clobbers more registers
 * than strictly necessary. Generates a lot more instructions than
 * strictly necessary if compiling to an immediate value.
 */
inline static
uint8_t table_element_cmp_eq(volatile freemcan_uint24_t *element,
                             const uint32_t value)
{
  uint8_t result_bool;
  asm volatile(
        "\n\t"

/* \todo */

        : /* output operands */
          [result] "=&r" (result_bool)
        : /* input operands */
          [elem] "b" (element),
          [valu] "r" (value)
               );
  return result_bool;
}

#else

/** Zero 8bit, 16bit, or 32bit unsigned integer */
inline static
void table_element_zero(volatile table_element_t *dest)
{
  *dest = 0;
}

inline static
void table_element_copy(volatile table_element_t *dest,
                        volatile table_element_t *source)
{
  *dest = *source;
}

/** Increment 8bit, 16bit, or 32bit unsigned integer */
inline static
void table_element_inc(volatile table_element_t *element)
{
  (*element)++;
}

/** Compare table element to a value for equality.
 *
 * \param element Pointer to the table element
 * \param value   Value to compare table element with.
 *
 * Note that #value has the same type as #*element here.
 */
inline static
uint8_t table_element_cmp_eq(volatile table_element_t *element,
                             const table_element_t value)
{
  return ((*element) == value);
}

#endif


#endif /* !TABLE_ELEMENT_H */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
