// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include "spi.h"
#include "dev_access.h"

void spi_init(spi_t *spi, spi_reg_t spi_reg, uint32_t speed) {
  spi->reg = spi_reg;
  spi->speed = speed;
}

void spi_send_byte_blocking(spi_t *spi, char c) {
  while(DEV_READ(spi->reg + SPI_STATUS_REG) & SPI_STATUS_TX_FULL);
  DEV_WRITE(spi->reg + SPI_TX_REG, c);
}

spi_status_t spi_get_status(spi_t *spi) {
   return (spi_status_t) DEV_READ(spi->reg + SPI_STATUS_REG);
}
