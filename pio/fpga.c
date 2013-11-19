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

#include <libpixi/pixi/fpga.h>
#include "Command.h"
#include "log.h"
#include <stdio.h>

static const char DefaultFpga[] = "/home/pixi-200/pixi.bin"; // TODO: belongs in ../share/pixi?

static int fpgaLoadFn (uint argc, char*const*const argv)
{
	if (argc > 2)
	{
		PIO_LOG_ERROR ("usage: %s [FPGAFILE]", argv[0]);
		return -EINVAL;
	}
	const char* filename = NULL;
	if (argc > 1)
		filename = argv[1];
	else
		filename = DefaultFpga;
	int result = pixi_pixiFpgaLoadFile(filename);
	if (result < 0)
	{
		PIO_ERROR(-result, "Could not load FPGA file from [%s]", filename);
		return result;
	}
	printf ("FPGA loaded\n");
	int64 version = pixi_pixiFpgaGetVersion();
	if (version < 0)
	{
		PIO_ERROR(-result, "Could not get loaded FPGA version");
		return result;
	}
	printf ("FPGA Version: %012llx\n", (ulonglong) version);
	return 0;
}
static Command fpgaLoadCmd =
{
	.name        = "load-fpga",
	.description = "load an FPGA image to the PiXi",
	.function    = fpgaLoadFn
};

static int fpgaGetVersionFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc != 1)
	{
		PIO_LOG_ERROR ("usage: %s", argv[0]);
		return -EINVAL;
	}
	int64 version = pixi_pixiFpgaGetVersion();
	if (version < 0)
	{
		PIO_ERROR(-version, "Error getting FPGA version");
		return version;
	}
	printf ("%012llx\n", (ulonglong) version);
	return 0;
}
static Command fpgaGetVersionCmd =
{
	.name        = "fpga-version",
	.description = "print the PiXi FPGA version",
	.function    = fpgaGetVersionFn
};


static int fpgaGetBuildTimeFn (uint argc, char*const*const argv)
{
	LIBPIXI_UNUSED(argv);
	if (argc != 1)
	{
		PIO_LOG_ERROR ("usage: %s", argv[0]);
		return -EINVAL;
	}
	int64 version = pixi_pixiFpgaGetVersion();
	if (version <= 0)
	{
		PIO_ERROR(-version, "Error getting FPGA version");
		return version;
	}
	time_t time = pixi_pixiFpgaVersionToTime (version);
	if (time < 0)
	{
		PIO_ERROR(-time, "Error converting FPGA version to time");
		return time;
	}
	// Use strftime %c to get localised time format
	char buf[80];
	size_t chars = strftime (buf, sizeof (buf), "%c\n", gmtime (&time));
	if (chars > 0)
		printf (buf);
	else
	{
		PIO_ERROR(-time, "Error formatting time value of %ld", time);
		return -EINVAL;
	}
	return 0;
}
static Command fpgaGetBuildTimeCmd =
{
	.name        = "fpga-build-time",
	.description = "print the PiXi FPGA build time",
	.function    = fpgaGetBuildTimeFn
};
static const Command* commands[] =
{
	&fpgaLoadCmd,
	&fpgaGetVersionCmd,
	&fpgaGetBuildTimeCmd
};


static CommandGroup pixiSpiGroup =
{
	.name      = "pixi-fpga",
	.count     = ARRAY_COUNT(commands),
	.commands  = commands,
	.nextGroup = NULL
};

static void PIO_CONSTRUCTOR (101) initGroup (void)
{
	addCommandGroup (&pixiSpiGroup);
}