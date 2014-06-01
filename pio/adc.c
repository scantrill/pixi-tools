/*
    pixi-tools: a set of software to interface with the Raspberry Pi
    and PiXi-200 hardware
    Copyright (C) 2013 Simon Cantrill

    pixi-tools is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <libpixi/pixi/adc.h>
#include <libpixi/util/string.h>
#include "Command.h"
#include "log.h"
#include <stdio.h>

static int adcReadFn (uint argc, char* argv[])
{
	if (argc != 2)
	{
		PIO_LOG_ERROR ("usage: %s CHANNEL", argv[0]);
		return -EINVAL;
	}

	uint adcChannel = pixi_parseLong (argv[1]);

	SpiDevice dev = SpiDeviceInit;
	int result = pixi_pixiAdcOpen(&dev);
	if (result < 0)
	{
		PIO_ERROR(-result, "Failed to open ADC SPI device");
		return result;
	}
	result = pixi_pixiAdcRead (&dev, adcChannel);
	pixi_spiClose (&dev);
	if (result < 0)
	{
		PIO_ERROR(-result, "ADC SPI read failed");
		return result;
	}

	printf ("%d\n", result);
	return 0;
}
static Command adcReadCmd =
{
	.name        = "adc-read",
	.description = "read an ADC channel",
	.function    = adcReadFn
};

static const Command* commands[] =
{
	&adcReadCmd,
};


static CommandGroup pixiAdcGroup =
{
	.name      = "pixi-adc",
	.count     = ARRAY_COUNT(commands),
	.commands  = commands,
	.nextGroup = NULL
};

static void PIO_CONSTRUCTOR (103) initGroup (void)
{
	addCommandGroup (&pixiAdcGroup);
}
