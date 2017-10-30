#ifndef _SINCOS_H
#define _SINCOS_H

typedef struct SinCos_t { float sin, cos; } SinCos;

extern void sinCos(float fRadians, SinCos *result);

#endif
