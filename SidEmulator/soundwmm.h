/*
 * soundwmm.h - Implementation of a Windows Waveout sound device.
 * Version 1.07 + stereo sid support
 *
 * Written by
 *  Lasse ™”rni <loorni@student.oulu.fi>
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *
 * Based on the DirectSound driver by
 *  Tibor Biczo <crown@mail.matav.hu>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */
#ifndef __SOUNDWMM_H__
#define __SOUNDWMM_H__

typedef unsigned short SWORD;

int  wmm_init(const char *param, int *speed, int *fragsize, int *fragnr, int *channels);
int  wmm_bufferspace(void);
int  wmm_write(SWORD *pbuf, size_t nr);
void wmm_close(void);

#endif // not __SOUNDWMM_H__
