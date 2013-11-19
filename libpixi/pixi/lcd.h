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

#ifndef libpixi_pixi_lcd_h__included
#define libpixi_pixi_lcd_h__included


#include <libpixi/pi/spi.h>

LIBPIXI_BEGIN_DECLS

///@defgroup PixiLcd PiXi-200 LCD panel interface
///@{

typedef struct LcdDevice
{
	SpiDevice spi;
} LcdDevice;

static inline void initLcdDevice (LcdDevice* device)
{
	device->spi = SpiDeviceInit;
}

///	Open the LcdDevice.
///	Return 0 on success, -errno on error
int pixi_lcdOpen (LcdDevice* device);

///	Close an SPI device opened via pixi_spiOpen()
///	@return 0 on success, or -errno on error
int pixi_lcdClose (LcdDevice* device);

///	Initialise the LCD panel
int pixi_lcdInit (LcdDevice* device);
int pixi_lcdInit1 (LcdDevice* device);

///	Set the brightness of the lcd. Valid values are in range [0,3]
int pixi_lcdSetBrightness (LcdDevice* device, uint value);

///	Clear the text
int pixi_lcdClear (LcdDevice* device);

///	Move cursor to @c x, @c y
int pixi_lcdSetCursorPos (LcdDevice* device, uint x, uint y);

///	Append @c str to the panel text
int pixi_lcdWriteStr (LcdDevice* device, const char* str);

///@} defgroup

LIBPIXI_END_DECLS

#endif // !defined libpixi_pixi_lcd_h__included