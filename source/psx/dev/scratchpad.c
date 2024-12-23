#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "psx/log.h"
#include "psx/dev/scratchpad.h"

psx_scratchpad_t *psx_scratchpad_create(void)
{
    return (psx_scratchpad_t *)malloc(sizeof(psx_scratchpad_t));
}

void psx_scratchpad_init(psx_scratchpad_t *scratchpad)
{
    memset(scratchpad, 0, sizeof(psx_scratchpad_t));

    scratchpad->io_base = PSX_SCRATCHPAD_BEGIN;
    scratchpad->io_size = PSX_SCRATCHPAD_SIZE;

    scratchpad->buf = (uint8_t *)malloc(PSX_SCRATCHPAD_SIZE);
}

uint32_t psx_scratchpad_read32(psx_scratchpad_t *scratchpad, uint32_t offset)
{
    return *((uint32_t *)(scratchpad->buf + offset));
}

uint16_t psx_scratchpad_read16(psx_scratchpad_t *scratchpad, uint32_t offset)
{
    return *((uint16_t *)(scratchpad->buf + offset));
}

uint8_t psx_scratchpad_read8(psx_scratchpad_t *scratchpad, uint32_t offset)
{
    return scratchpad->buf[offset];
}

void psx_scratchpad_write32(psx_scratchpad_t *scratchpad, uint32_t offset, uint32_t value)
{
    *((uint32_t *)(scratchpad->buf + offset)) = value;
}

void psx_scratchpad_write16(psx_scratchpad_t *scratchpad, uint32_t offset, uint16_t value)
{
    *((uint16_t *)(scratchpad->buf + offset)) = value;
}

void psx_scratchpad_write8(psx_scratchpad_t *scratchpad, uint32_t offset, uint8_t value)
{
    scratchpad->buf[offset] = value;
}

void psx_scratchpad_destroy(psx_scratchpad_t *scratchpad)
{
    free(scratchpad->buf);
    free(scratchpad);
}