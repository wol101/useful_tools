// BigEndian.h - Macros and C++ routines to normalise everything to BigEndian

#ifndef BigEndian_h
#define BigEndian_h

#include <machine/endian.h>
#include <machine/types.h>

// don't bother to change if BIG_ENDIAN
#if BYTE_ORDER == BIG_ENDIAN

#define BigEndian(x) (x)

#endif

#if BYTE_ORDER == LITTLE_ENDIAN

inline int16_t BigEndian(int16_t v)
{
  int16_t r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 1;

  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

inline u_int16_t BigEndian(u_int16_t v)
{
  u_int16_t r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 1;

  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

inline int32_t BigEndian(int32_t v)
{
  int32_t r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 3;

  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

inline u_int32_t BigEndian(u_int32_t v)
{
  u_int32_t r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 3;

  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

inline float BigEndian(float v)
{
  float r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 3;

  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

inline double BigEndian(double v)
{
  float r;
  int8_t *p0 =  (int8_t *)&v;
  int8_t *p1 =  (int8_t *)&r + 7;

  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1-- = *p0++;
  *p1 = *p0;
  
  return r;
}

#endif

#endif
