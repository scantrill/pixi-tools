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

#include <libpixi/pi/gpio.h>
#include "Command.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

static int gpioPinsFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc > 1)
	{
		PIO_LOG_ERROR ("usage: %s", argv[0]);
		return -EINVAL;
	}

	int result = pixi_gpioMapRegisters();
	if (result < 0)
	{
		PIO_ERROR (-result, "could not map gpio registers");
		return result;
	}
	GpioState states[GpioNumPins];
	result = pixi_gpioPhysGetPinStates (states, ARRAY_COUNT(states));
	if (result < 0)
	{
		PIO_ERROR (-result, "could not get gpio pin states");
		return result;
	}
	const char header[] = "gpio | mode | value\n";
	const char row[]    = "%4u | %4x | %5x\n";
	printf (header);
	for (uint gpio = 0; gpio < ARRAY_COUNT(states); gpio++)
	{
		const GpioState* state = &states[gpio];
		printf (row,
			gpio,
			state->direction,
			state->value
			);
	}
	return 0;
}
static Command gpioPinsCmd =
{
	.name        = "gpio-pins",
	.description = "display state of gpio pins",
	.function    = gpioPinsFn
};

static int listExportsFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc > 1)
	{
		PIO_LOG_ERROR ("usage: %s", argv[0]);
		return -EINVAL;
	}

	GpioState states[64];
	int result = pixi_gpioSysGetPinStates (states, ARRAY_COUNT(states));
	printf ("%d gpio exports\n", result);
	const char header[] = "gpio | direction | value |    edge | activeLow\n";
	const char row[]    = "%4u | %9s | %5d | %7s | %9d\n";
	printf (header);
	for (uint gpio = 0; gpio < ARRAY_COUNT(states); gpio++)
	{
		const GpioState* state = &states[gpio];
		if (!state->exported)
			continue;
		printf (row,
			gpio,
			pixi_gpioDirectionToStr (state->direction),
			state->value,
			pixi_gpioEdgeToStr (state->edge),
			state->activeLow
			);
	}
	return 0;
}
static Command listExportsCmd =
{
	.name        = "exports",
	.description = "display state of gpios",
	.function    = listExportsFn
};


static int exportGpioFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc != 3)
	{
		PIO_LOG_ERROR ("usage: %s PIN DIRECTION", argv[0]);
		return -EINVAL;
	}
	int gpio = atoi (argv[1]);
	const char* directionStr = argv[2];
	Direction direction = pixi_gpioStrToDirection (directionStr);
	if ((int) direction < 0)
	{
		PIO_LOG_ERROR ("[%s] is not a valid direction", directionStr);
		return -EINVAL;
	}
	int result = pixi_gpioSysExportPin (gpio, direction);
	if (result < 0)
	{
		PIO_ERROR (-result, "Export of gpio %d (%s) failed", gpio, directionStr);
		return -EINVAL;
	}
	// TODO: change owner of exported file
	// TODO: 'force' mode. Force re-export if already exported (EBUSY)
	return 0;
}
static Command exportGpioCmd =
{
	.name        = "export",
	.description = "export a gpio pin",
	.function    = exportGpioFn
};


static int unexportGpioFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc != 2)
	{
		PIO_LOG_ERROR ("usage: %s PIN", argv[0]);
		return -EINVAL;
	}
	int gpio = atoi (argv[1]);
	int result = pixi_gpioSysUnexportPin (gpio);
	if (result < 0)
	{
		PIO_ERROR (-result, "Unexport of gpio %d failed", gpio);
		return -EINVAL;
	}
	return 0;
}
static Command unexportGpioCmd =
{
	.name        = "unexport",
	.description = "unexport a gpio pin",
	.function    = unexportGpioFn
};


static const Command* gpioCommands[] =
{
	&gpioPinsCmd,
	&listExportsCmd,
	&exportGpioCmd,
	&unexportGpioCmd
};

CommandGroup gpioGroup =
{
	.name      = "gpio",
	.count     = ARRAY_COUNT(gpioCommands),
	.commands  = gpioCommands,
	.nextGroup = NULL
};