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

%module pi

// strip pixi_ prefix for python symbols
%rename("%(strip:[pixi_])s") "";
%rename("%(regex:/(pixi_piGpio)(.*)/gpio\\2/)s") "";
%feature("autodoc", "3");

%{
#include <libpixi/common.h>
#include <libpixi/libpixi.h>
#include <libpixi/pi/gpio.h>
#include <libpixi/pi/i2c.h>
#include <libpixi/pi/spi.h>
#include <libpixi/version.h>
%}
%include <stdint.i>
%include <libpixi/common.h>
%include <libpixi/libpixi.h>
%include <libpixi/pi/gpio.h>
%include <libpixi/pi/i2c.h>
%include <libpixi/pi/spi.h>
%include <libpixi/version.h>

%pythoncode %{
import pixitools
pixitools._rewrap (vars(), vars (_pi))

def initLib (expectedVersion = LIBPIXI_VERSION_INT):
	return _pi.initLib (expectedVersion)
%}
