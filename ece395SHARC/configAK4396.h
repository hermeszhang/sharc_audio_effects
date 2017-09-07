#ifndef _CONFIGAK4396_H
#define _CONFIGAK4396_H

#include <Cdef21489.h>
#include <SRU.h>

#include "globals.h"

#define SELECT_SPI_SLAVE(select) (*pSPIFLG &= ~(DS0EN<<8))
#define DESELECT_SPI_SLAVE(select) (*pSPIFLG |= (DS0EN<<8))

void configAK4396(unsigned int address, unsigned int data);

#endif

