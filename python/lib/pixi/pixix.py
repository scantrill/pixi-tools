#    pixi-tools: a set of software to interface with the Raspberry Pi
#    and PiXi-200 hardware
#    Copyright (C) 2013 Simon Cantrill
#
#    pixi-tools is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from __future__ import print_function
from pixitools import pi, pixi
from os import getenv, strerror
from time import sleep
import logging

log = logging.getLogger(__name__)
info = log.info
debug = log.debug

def pause():
	sleep (0.1)

LcdLineLen = 40

class Lcd (object):
	def __init__(self, filename = None):
		self.filename = filename # persistent state, since we cannot read from the panel
		pixi.lcdEnable()
		self.lines = [' ' * LcdLineLen, ' ' * LcdLineLen]
		self.loadState()

	def __del__(self):
		self.close()

	def loadState (self):
		try:
			state = open (self.filename)
			lines = state.readlines()
			self.lines = [self.trim (lines[0]), self.trim (lines[1])]
		except Exception:
			pass

	def saveState (self):
		try:
			state = open (self.filename, "w")
			state.write ('\n'.join (self.lines))
		except Exception:
			pass

	def close (self):
		pass #pixi.closePixi()

	def write (self, text):
#		info ("Writing panel text [%s]", text)
		pixi.lcdWriteStr (text)
		pause()

	def flush (self):
		pass

	def clear (self):
		pixi.lcdClear()
		pause()

	def setCursorPos (self, x, y):
		pixi.lcdSetCursorPos (x, y)
		pause()

	def setBrightness (self, brightness):
		pixi.lcdSetBrightness (brightness)
		pause()

	def setText (self, text):
		# TODO: only send changes, instead of everything
		lines = text.split('\n')
		if len (lines) == 0:
			lines = ['','']
		elif len (lines) == 1:
			lines.append ('')
		elif len (lines) > 2:
			lines = lines[:2]
		lines[0] = self.trim (lines[0])
		lines[1] = self.trim (lines[1])
		self.lines = lines
		self.writeBuffer()

	def trim (self, text):
		if len (text) > LcdLineLen:
			text = text[:LcdLineLen]
		elif len (text) < LcdLineLen:
			text += ' ' * (LcdLineLen - len (text))
		return text

	def echo (self, text):
		text = self.trim (text)
		lines = self.lines
		lines[0] = lines[1]
		lines[1] = text
		self.writeBuffer()

	def writeBuffer (self):
		lines = self.lines
		self.setCursorPos (0, 0)
		self.write (lines[0] + lines[1])
		self.saveState()

ServoA1 = 0x40
ServoA2 = 0x41
ServoB1 = 0x46
ServoB2 = 0x47
