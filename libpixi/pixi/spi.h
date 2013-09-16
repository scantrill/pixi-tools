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

#ifndef pio_spi_h__included
#define pio_spi_h__included


#include <libpixi/pi/spi.h>

LIBPIXI_BEGIN_DECLS

///@defgroup PiXiSpi PiXi SPI interface
///@{

enum
{
	PixiSpiChannel = 0,
	PixiSpiSpeed   = 8000000,

	PixiSpiEnableWrite8  = 0x40,
	PixiSpiEnableWrite16 = 0x50,
	PixiSpiEnableWrite32 = 0x60,
	PixiSpiEnableRead16  = 0x80
};

///	Open the Pi SPI channel to the pixi. When done with @c device,
///	call pixi_spiClose().
///	@return 0 on success, or -errno on error
int pixi_pixiSpiOpen (SpiDevice* device);

///	Write a @c value to an @c address.
///	@return the read back value on success, or -errno on error
int pixi_pixiSpiWriteValue16 (SpiDevice* device, uint address, ushort value);

///	Read a value from a @c address.
///	@return the (uint16) value on success, or -errno on error
int pixi_pixiSpiReadValue16 (SpiDevice* device, uint address);

///@} defgroup

LIBPIXI_END_DECLS

#endif // !defined pio_spi_h__included
