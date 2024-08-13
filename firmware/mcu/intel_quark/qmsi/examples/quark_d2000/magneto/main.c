/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Magnetometer
 *
 * This application will read the magneto data from the onboard sensor on the
 * Intel(R) Quark(TM) D2000 development platform and print it to the console
 * every 125 milliseconds. The app will complete once it has read 500 samples.
 */

#include <math.h>
#include "clk.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "bmx1xx/bmx1xx.h"

#define NUM_SAMPLES (500)
#define M_PI 3.14159265358979323846

/* Convert degrees into compass direction. */
static const char *degrees_to_direction(unsigned int deg)
{
	if (deg >= 360) {
		deg %= 360;
	}

	if (deg >= 338 || deg < 23) {
		return "N";
	} else if (deg < 68) {
		return "NE";
	} else if (deg < 113) {
		return "E";
	} else if (deg < 158) {
		return "SE";
	} else if (deg < 203) {
		return "S";
	} else if (deg < 248) {
		return "SW";
	} else if (deg < 293) {
		return "W";
	} else {
		return "NW";
	}
}

int main(void)
{
	bmx1xx_setup_config_t cfg;
	bmx1xx_mag_t mag = {0};
	double heading;
	int deg;
	uint32_t count = 0;

	QM_PUTS("Starting: Magnetometer");

	/* Initialise the magneto. */
	cfg.pos = BMC150_J14_POS_0;

	if (0 != bmx1xx_init(cfg)) {
		QM_PUTS("Error: Unable to initialise BMC150.");
		return 1;
	}

	/* Activate the magneto. */
	if (0 != bmx1xx_mag_set_power(BMX1XX_MAG_POWER_ACTIVE)) {
		QM_PUTS("Error: Unable to set BMC150 power state.");
		return 1;
	}

	/* Set the magneto to high accuracy. */
	if (0 != bmx1xx_mag_set_preset(BMX1XX_MAG_PRESET_HIGH_ACCURACY)) {
		QM_PUTS("Error: Unable to set BBMC150 to high accuracy.");
		return 1;
	}

	for (count = 0; count < NUM_SAMPLES; count++) {
		clk_sys_udelay(125000);

		/* Read the value from the magneto. */
		bmx1xx_read_mag(&mag);

		/* Calculate the heading. */
		heading = atan2(mag.y, mag.x);
		if (heading < 0) {
			heading += (2 * M_PI);
		}

		/* Convert the heading into degrees. */
		deg = (int)((heading * 180) / M_PI);

		QM_PRINTF("x: %d y: %d z: %d deg: %d direction: %s\n", mag.x,
			  mag.y, mag.z, deg, degrees_to_direction(deg));
	}

	QM_PUTS("Finished: Magnetometer");

	return 0;
}
