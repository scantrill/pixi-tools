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
#include <libpixi/pixi/spi.h>
#include <libpixi/pi/gpio.h>
#include <libpixi/util/log.h>
#include <stdio.h>

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#define PROG_PIN 6                                      // GPIO(6)
#define INIT_PIN 2                                      // GPIO(2) Note this pin is different for a rev1 or rev 2 board but wiringPi sorts this out very nicely!
#define CCLK_PIN 0                                      // GPIO(0)
#define DATA_PIN 1                                      // GPIO(1)


static void pinMode (int pin, int mode)
{
	pixi_gpioSetPinMode(pin, mode ? DirectionOut : DirectionIn);
}

static void digitalWrite (int pin, int value)
{
	pixi_gpioWritePin(pin, value);
}

static int digitalRead (int pin)
{
	return pixi_gpioReadPin(pin);
}

int64 pixi_pixiFpgaGetVersion (void)
{
	SpiDevice spi = SpiDeviceInit;
	int result = pixi_pixiSpiOpen (&spi);
	if (result < 0)
		return result;

	int h = pixi_pixiSpiReadValue16 (&spi, 0x02);
	int m = pixi_pixiSpiReadValue16 (&spi, 0x01);
	int l = pixi_pixiSpiReadValue16 (&spi, 0x00);
	LIBPIXI_LOG_DEBUG("Got PiXi FPGA version %04x,%04x,%04x", h, m, l);

	pixi_spiClose (&spi);
	return
		((uint64) h << 32) |
		((uint64) m << 16) |
		((uint64) l);
}

int pixi_pixiFpgaLoadBuffer (const Buffer* _buffer)
{
	int result = pixi_gpioMapRegisters();
	if (result < 0)
	{
		LIBPIXI_LOG_ERROR("Unable map GPIO registers");
		return result;
	}

	LIBPIXI_LOG_INFO("Setting pin I/O Direction...");
	pinMode(PROG_PIN, OUTPUT);
	pinMode(INIT_PIN, INPUT);
	pinMode(CCLK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);

//	int demo_build = spi_single_read(0, 0xff); // Check if a demo build is currently active in the FPGA

	LIBPIXI_LOG_INFO("Setting PROG low...");
	digitalWrite(PROG_PIN, LOW);

	// ***** Hold PROG low line for a short while *****
	usleep(1000);

	LIBPIXI_LOG_INFO("Setting PROG high...");
	digitalWrite(PROG_PIN, HIGH);

	LIBPIXI_LOG_INFO("Wait for INIT...");
	int timeout = 100;
	while ((timeout > 1) && (!(digitalRead(INIT_PIN) == HIGH)))
	{
		usleep(1000);
		timeout--;
	}

	if (digitalRead(INIT_PIN) == HIGH)
		LIBPIXI_LOG_INFO("Ready to program PiXi...");
	else
	{
		LIBPIXI_LOG_ERROR("INIT did not go high!");
		return -EINVAL; // ??
	}

	uint byte_counter = 0;
	int percent_complete = 0;

	usleep(1000000);

	const char* buffer = _buffer->memory;
	const size_t bytes_read = _buffer->size;
	const size_t bytes_readDIV10 = bytes_read / 10;

	// ***** Download to FPGA *****
	const char* percentFormat = "FPGA load %3d%% complete...";
	LIBPIXI_LOG_INFO(percentFormat, percent_complete);
	for (size_t i = 0; i < bytes_read; i++)
	{
		uchar byte = *(buffer + i);
		uchar shift_byte = byte;
//		uint repeat = 1;

//		while (repeat--)
		{
			for (uint j = 0; j < 8; j++)       //data goes out serially
			{
				// Set CCLK = '0'
				digitalWrite(CCLK_PIN, LOW);

				if (!(shift_byte & 0x80))
					digitalWrite(DATA_PIN, LOW);
				else
					digitalWrite(DATA_PIN, HIGH);

				// Set CCLK = '1', PROG = '1', DIN = data
				digitalWrite(CCLK_PIN, HIGH);

				shift_byte = shift_byte << 1;
			}
			byte_counter++;
		}

		if (byte_counter >= bytes_readDIV10)
		{
			percent_complete += 10;
			LIBPIXI_LOG_INFO(percentFormat, percent_complete);
			byte_counter -= bytes_readDIV10;
		}
	}

	// Need to contine clocking CCLK for a little while after download...
	for (uint j = 0; j < 8; j++)
	{
		digitalWrite(CCLK_PIN, LOW);
		digitalWrite(CCLK_PIN, HIGH);
	}

	usleep(100000);

	return 0;
}

int pixi_pixiFpgaLoadFile (const char* filename)
{
	LIBPIXI_PRECONDITION_NOT_NULL(filename);

	Buffer buffer = BufferInit;
	int result = pixi_fileLoadContents (filename, &buffer);
	if (result < 0)
	{
		LIBPIXI_ERROR(-result, "Could not load file [%s]", filename);
		return result;
	}
	if (buffer.size > 0) // TODO: check for some larger minimum size?
	{
		LIBPIXI_LOG_DEBUG("Loaded FPGA image [%s] size=%zu", filename, buffer.size);
		result = pixi_pixiFpgaLoadBuffer (&buffer);
		if (result < 0)
			LIBPIXI_ERROR(-result, "Could not load FPGA taken from file [%s]", filename);
	}
	else
	{
		LIBPIXI_LOG_ERROR("FPGA image [%s] is too small: size=%zu", filename, buffer.size);
		result = -ERANGE; // ??
	}
	free (buffer.memory);

	return result;
}
