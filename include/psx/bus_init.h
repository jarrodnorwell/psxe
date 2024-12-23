#ifndef BUS_INIT_H
#define BUS_INIT_H

#include "psx/dev/cdrom/cdrom.h"
#include "psx/dev/bios.h"
#include "psx/dev/ram.h"
#include "psx/dev/dma.h"
#include "psx/dev/exp1.h"
#include "psx/dev/exp2.h"
#include "psx/dev/mc1.h"
#include "psx/dev/mc2.h"
#include "psx/dev/mc3.h"
#include "psx/dev/ic.h"
#include "psx/dev/scratchpad.h"
#include "psx/dev/gpu.h"
#include "psx/dev/spu.h"
#include "psx/dev/timer.h"
#include "psx/dev/pad.h"
#include "psx/dev/mdec.h"

struct psx_bus_t
{
    psx_bios_t *bios;
    psx_ram_t *ram;
    psx_dma_t *dma;
    psx_exp1_t *exp1;
    psx_exp2_t *exp2;
    psx_mc1_t *mc1;
    psx_mc2_t *mc2;
    psx_mc3_t *mc3;
    psx_ic_t *ic;
    psx_scratchpad_t *scratchpad;
    psx_gpu_t *gpu;
    psx_spu_t *spu;
    psx_timer_t *timer;
    psx_cdrom_t *cdrom;
    psx_pad_t *pad;
    psx_mdec_t *mdec;

    uint32_t access_cycles;
};

void psx_bus_init_bios(psx_bus_t *, psx_bios_t *);
void psx_bus_init_ram(psx_bus_t *, psx_ram_t *);
void psx_bus_init_dma(psx_bus_t *, psx_dma_t *);
void psx_bus_init_exp1(psx_bus_t *, psx_exp1_t *);
void psx_bus_init_exp2(psx_bus_t *, psx_exp2_t *);
void psx_bus_init_mc1(psx_bus_t *, psx_mc1_t *);
void psx_bus_init_mc2(psx_bus_t *, psx_mc2_t *);
void psx_bus_init_mc3(psx_bus_t *, psx_mc3_t *);
void psx_bus_init_ic(psx_bus_t *, psx_ic_t *);
void psx_bus_init_scratchpad(psx_bus_t *, psx_scratchpad_t *);
void psx_bus_init_gpu(psx_bus_t *, psx_gpu_t *);
void psx_bus_init_spu(psx_bus_t *, psx_spu_t *);
void psx_bus_init_timer(psx_bus_t *, psx_timer_t *);
void psx_bus_init_cdrom(psx_bus_t *, psx_cdrom_t *);
void psx_bus_init_pad(psx_bus_t *, psx_pad_t *);
void psx_bus_init_mdec(psx_bus_t *, psx_mdec_t *);

#endif