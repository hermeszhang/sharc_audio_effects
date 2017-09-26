#ifndef _READPOTVALUES_H
#define _READPOTVALUES_H

#include <Cdef21489.h>
#include <SRU.h>

#include "globals.h"

#define SELECT_SPI_SLAVE(select) (*pSPIFLG &= ~(select<<8))
#define DESELECT_SPI_SLAVE(select) (*pSPIFLG |= (select<<8))

int readPotValues(void);

#endif
