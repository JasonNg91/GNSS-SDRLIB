#pragma once

/*
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
 * Copyright (C) 2012-2013 by Steve Markgraf <steve@steve-m.de>
 * Copyright (C) 2012 by Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <stdint.h>
#include "IFx3Device.h"
#include "Fx3Factory.h"
#include "fifo.h"

    /*-GNSS-SDRLIB----------------------------------------------------------------*/
#include "../../../src/sdr.h"


#define NUT2NT_DATABUFF_SIZE    (128*1024) // The buffer size is not software limited. Can be any

int nut_init(void);
void nut_quit(void);
int nut_initconf(void);
int nut_start(void);
void nut_exp(uint8_t* buf, int n, char* expbuf);
void nut_getbuff(uint64_t buffloc, int n, char* expbuf);
void nut_pushtomembuf(void);
void readloop();
int read_from_NUT4NT(void* dst);
/*----------------------------------------------------------------------------*/

typedef struct rtlsdr_dev rtlsdr_dev_t;
