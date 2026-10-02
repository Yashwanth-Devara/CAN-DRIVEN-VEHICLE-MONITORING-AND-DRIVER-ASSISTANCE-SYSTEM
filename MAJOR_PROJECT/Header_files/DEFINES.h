#include "types.h"


/* ---------- SINGLE BIT OPERATIONS ---------- */

/* Set a specific bit to 1 */
#define SETBIT(WORD,BIT) \
        (WORD |= 1 << BIT)


/* Clear a specific bit to 0 */
#define CLRBIT(WORD,BIT) \
        (WORD &= ~(1 << BIT))


/* Toggle a specific bit */
#define CPLBIT(WORD,BIT) \
        (WORD ^= 1 << BIT)


/* Write 0 or 1 to a specific bit */
#define WRITEBIT(WORD,BITPOS,BITLEVEL) \
        WORD = ((WORD & ~(1 << BITPOS)) | \
               (BITLEVEL << BITPOS))


/* Read the value of a specific bit */
#define READBIT(WORD,BITPOS) \
        ((WORD >> BITPOS) & 1)


/* ---------- BIT COPY OPERATION ---------- */

/* Copy one bit from source word to destination bit */
#define READWRITEBIT(DWORD,DBIT,SWORD,SBIT) \
        DWORD = ((DWORD & ~(1 << DBIT)) | \
                (((SWORD >> SBIT) & 1) << DBIT))


/* ---------- NIBBLE OPERATIONS ---------- */

/* Write a 4-bit value into a word */
#define WRITENIBBLE(WORD,BITSTARTPOS,VALUE) \
        WORD = ((WORD & ~(15 << BITSTARTPOS)) | \
               (VALUE << BITSTARTPOS))


/* Read a 4-bit value from a word */
#define READNIBBLE(WORD,BITSTARTPOS) \
        ((WORD >> BITSTARTPOS) & 15)


/* ---------- BYTE OPERATIONS ---------- */

/* Write an 8-bit value into a word */
#define WRITEBYTE(WORD,BITSTARTPOS,BYTE) \
        WORD = ((WORD & ~((u32)255 << BITSTARTPOS)) | \
               ((u32)BYTE << BITSTARTPOS))


/* Read an 8-bit value from a word */
#define READBYTE(WORD,BITSTARTPOS) \
        ((WORD >> BITSTARTPOS) & 255)


/* ---------- MULTIPLE BIT OPERATION ---------- */

/* Write N bits starting from a specified bit position */
#define WRITENBITS(WORD,BITSTARTPOS,NBITS,VAL) \
        WORD = ((WORD & ~(((1 << NBITS) - 1) << BITATARTPOS)) | \
               (VAL << BITSTARTPOS))