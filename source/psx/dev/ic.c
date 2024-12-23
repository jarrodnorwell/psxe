#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "psx/dev/ic.h"

#include "psx/log.h"

psx_ic_t *psx_ic_create(void)
{
    return (psx_ic_t *)malloc(sizeof(psx_ic_t));
}

void psx_ic_init(psx_ic_t *ic, psx_cpu_t *cpu)
{
    memset(ic, 0, sizeof(psx_ic_t));

    ic->io_base = PSX_IC_BEGIN;
    ic->io_size = PSX_IC_SIZE;

    ic->stat = 0x00000000;
    ic->mask = 0x00000000;

    ic->cpu = cpu;
}

uint32_t psx_ic_read32(psx_ic_t *ic, uint32_t offset)
{
    switch (offset)
    {
    case 0x00:
        return ic->stat;
    case 0x04:
        return ic->mask;
    }

    log_fatal("Unhandled 32-bit IC read at offset %08x", offset);

    return 0x0;
}

uint16_t psx_ic_read16(psx_ic_t *ic, uint32_t offset)
{
    switch (offset)
    {
    case 0x00:
        return (ic->stat >> 0) & 0xffff;
    case 0x02:
        return (ic->stat >> 16) & 0xffff;
    case 0x04:
        return (ic->mask >> 0) & 0xffff;
    case 0x06:
        return (ic->mask >> 16) & 0xffff;
    }

    return 0x0;
}

uint8_t psx_ic_read8(psx_ic_t *ic, uint32_t offset)
{
    switch (offset)
    {
    case 0x00:
        return (ic->stat >> 0) & 0xff;
    case 0x01:
        return (ic->stat >> 8) & 0xff;
    case 0x02:
        return (ic->stat >> 16) & 0xff;
    case 0x03:
        return (ic->stat >> 24) & 0xff;
    case 0x04:
        return (ic->mask >> 0) & 0xff;
    case 0x05:
        return (ic->mask >> 8) & 0xff;
    case 0x06:
        return (ic->mask >> 16) & 0xff;
    case 0x07:
        return (ic->mask >> 24) & 0xff;
    }

    return 0x0;
}

void psx_ic_write32(psx_ic_t *ic, uint32_t offset, uint32_t value)
{
    switch (offset)
    {
    case 0x00:
        ic->stat &= value;
        break;
    case 0x04:
        ic->mask = value;
        break;

    default:
    {
        log_fatal("Unhandled 32-bit IC write at offset %08x (%08x)", offset, value);
    }
    break;
    }

    // Emulate acknowledge
    if (!(ic->stat & ic->mask))
    {
        ic->cpu->cop0_r[COP0_CAUSE] &= ~SR_IM2;
    }
    else
    {
        psx_cpu_set_irq_pending(ic->cpu);
    }
}

void psx_ic_write16(psx_ic_t *ic, uint32_t offset, uint16_t value)
{
    switch (offset)
    {
    case 0x00:
        ic->stat &= ((uint32_t)value) << 0;
        break;
    case 0x02:
        ic->stat &= ((uint32_t)value) << 16;
        break;
    case 0x04:
        ic->mask = ((uint32_t)value) << 0;
        break;
    case 0x06:
        ic->mask = ((uint32_t)value) << 16;
        break;
    }

    // Emulate acknowledge
    if (!(ic->stat & ic->mask))
    {
        ic->cpu->cop0_r[COP0_CAUSE] &= ~SR_IM2;
    }
    else
    {
        psx_cpu_set_irq_pending(ic->cpu);
    }
}

void psx_ic_write8(psx_ic_t *ic, uint32_t offset, uint8_t value)
{
    switch (offset)
    {
    case 0x00:
        ic->stat &= ((uint32_t)value) << 0;
        break;
    case 0x01:
        ic->stat &= ((uint32_t)value) << 8;
        break;
    case 0x02:
        ic->stat &= ((uint32_t)value) << 16;
        break;
    case 0x03:
        ic->stat &= ((uint32_t)value) << 24;
        break;
    case 0x04:
        ic->mask = ((uint32_t)value) << 0;
        break;
    case 0x05:
        ic->mask = ((uint32_t)value) << 8;
        break;
    case 0x06:
        ic->mask = ((uint32_t)value) << 16;
        break;
    case 0x07:
        ic->mask = ((uint32_t)value) << 24;
        break;
    }

    // Emulate acknowledge
    if (!(ic->stat & ic->mask))
    {
        ic->cpu->cop0_r[COP0_CAUSE] &= ~SR_IM2;
    }
    else
    {
        psx_cpu_set_irq_pending(ic->cpu);
    }
}

void psx_ic_irq(psx_ic_t *ic, int id)
{
    ic->stat |= id;

    if (ic->mask & ic->stat)
        psx_cpu_set_irq_pending(ic->cpu);
}

void psx_ic_destroy(psx_ic_t *ic)
{
    free(ic);
}