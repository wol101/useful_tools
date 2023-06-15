// ByteSwap.h - routines to provide byte swapping

#ifndef ByteSwap_h
#define ByteSwap_h

/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |               \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
/* Swap bytes in 64 bit value.  */
#define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)                                   \
      | (((x) & 0x00ff000000000000ull) >> 40)                                 \
      | (((x) & 0x0000ff0000000000ull) >> 24)                                 \
      | (((x) & 0x000000ff00000000ull) >> 8)                                  \
      | (((x) & 0x00000000ff000000ull) << 8)                                  \
      | (((x) & 0x0000000000ff0000ull) << 24)                                 \
      | (((x) & 0x000000000000ff00ull) << 40)                                 \
      | (((x) & 0x00000000000000ffull) << 56))


inline short ByteSwap(short v)
{
    unsigned short *p = (unsigned short *)&v;
    *p = __bswap_constant_16(*p);
    return v;
}    

inline int ByteSwap(int v)
{
    unsigned int *p = (unsigned int *)&v;
    *p = __bswap_constant_32(*p);
    return v;
}    

inline float ByteSwap(float v)
{
    unsigned int *p = (unsigned int *)&v;
    *p = __bswap_constant_32(*p);
    return v;
}    

inline double ByteSwap(double v)
{
    unsigned long long *p = (unsigned long long *)&v;
    *p = __bswap_constant_64(*p);
    return v;
}    

#endif

