/*
 * soundwmm.c - Implementation of a Windows Waveout sound device.
 * Version 1.07 + stereo sid support
 *
 * Written by
 *  Lasse ��rni <loorni@student.oulu.fi>
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
#include "stdafx.h"
#include <stdio.h>

#include <windows.h>
#include <mmsystem.h>
#include <string.h>

#include "soundwmm.h"

/* ------------------------------------------------------------------------ */
static DWORD buffer_size;        /* Buffer size.  */
static int fragment_size;        /* Fragment size.  */
static int fragment_bytesize;    /* Fragment size in bytes.  */
static int num_fragments;        /* Number of fragments */
static int num_of_channels;      /* Channels */
static int is16bit;              /*  Flag: is soundcard a 16bit or 8bit card? */
static HGLOBAL hbuffer = NULL;   /* Handle to sound buffer */
static LPSTR lpbuffer = NULL;    /* Pointer to sound buffer */
static HWAVEOUT hwaveout = NULL; /* Handle of waveout device */
static WAVEHDR wavehdr;          /* Wave header */
static MMTIME mmtime;
static WAVEFORMATEX wavfmt;      /* Wave format */

/* Initialization flags */
static int sndinitted = 0;
static int headerprepared = 0;
static int beginperiod = 0;

/* Buffer writing pos */
static int write_cursor = 0;

/* Playposition subtract */
static int play_cursor_subtract = 0;

/* Magic offset to buffer */
static int play_cursor_offset = 0;

/* Timer callback */
static TIMECAPS wmm_tc;
static unsigned wmm_timer_id = 0;

/* Inactivity timer (for clearing the buffer) */
static int   inactivity_timer;
static SWORD last_buffered_sample[2];

/* ------------------------------------------------------------------------
 * The job of the timer callback is only to clear the buffer if enough
 * inactivity from wmm_write()
 */
static void CALLBACK wmm_timercallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (!sndinitted)
        return;

    if (wavehdr.dwFlags & WHDR_DONE)
       return; /* Buffer stopped playing */

    if (!headerprepared)
        return;

    /*
     * Increment inactivity timer. If inactive for a whole buffer, stop
     * playing
     */
    inactivity_timer++;
    if (inactivity_timer >= num_fragments) 
	{
        waveOutReset(hwaveout);
        waveOutUnprepareHeader(hwaveout, &wavehdr, sizeof(WAVEHDR));
        headerprepared = 0;
        wavehdr.dwFlags |= WHDR_DONE;
	}
}


/* ------------------------------------------------------------------------
 * Init WMM
 */
int wmm_init(const char *param, int *speed, int *fragsize, int *fragnr, int *channels)
{
    DWORD dwVersion;

    num_of_channels = *channels;

    /* If wanted to re-initialize, shutdown first */
    wmm_close();

    /* Assume: 16bit successful */
    is16bit = 1;

    /* Set sound buffer properties */
    memset(&wavfmt, 0, sizeof wavfmt);
    wavfmt.wFormatTag = WAVE_FORMAT_PCM;
    wavfmt.nChannels = *channels;
    wavfmt.wBitsPerSample = 16;
    wavfmt.nSamplesPerSec = *speed;
    wavfmt.nAvgBytesPerSec = *speed * 2 * *channels;
    wavfmt.nBlockAlign = 2 * *channels;


    /* Try to open as 16bit */
    if (waveOutOpen(&hwaveout, WAVE_MAPPER, &wavfmt, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC) == MMSYSERR_NOERROR)
        goto WAVEOUT_OK;


    /* If not successful, then try 8-bit */
    is16bit = 0;
    wavfmt.wBitsPerSample = 8;
    wavfmt.nAvgBytesPerSec = *speed* *channels;
    wavfmt.nBlockAlign = 1 * *channels;

    /* Try to open as 8bit */
    if (waveOutOpen(&hwaveout, WAVE_MAPPER, &wavfmt, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC) != MMSYSERR_NOERROR) 
	{
        wmm_close();
        return -1;
    }
    WAVEOUT_OK:

    /* Calculate buffer size */
    fragment_size = *fragsize;
    fragment_bytesize = fragment_size * (is16bit ? sizeof(SWORD) : 1) * num_of_channels;
    num_fragments = *fragnr;
    buffer_size = fragment_bytesize * num_fragments;

    /*
     * Set magic buffer pos. offset (it seems that the play position that
     * Windows tells us is not entirely right)
     */
    play_cursor_offset = *speed / 16;
    if (is16bit)
        play_cursor_offset <<= 1;
    if (play_cursor_offset >= (int)buffer_size)
        play_cursor_offset = 0;

    /* If we're on Windows 2000/ME(?), no magic offset */
    dwVersion = GetVersion();
    if ((dwVersion & 0xff) >= 5 || ((dwVersion & 0xff) == 4 && ((dwVersion >> 8 & 0xff) >= 90)))
	{
		play_cursor_offset = 0;
	}

    /* Reset writing pos, wrapping subtract, inactivity timer */
    write_cursor = buffer_size - fragment_bytesize;
    play_cursor_subtract = 0;
    inactivity_timer = 0;

    /* Allocate sound buffer */
    hbuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, buffer_size);

    if (!hbuffer) 
	{
        wmm_close();
     	return -1;
    }

    lpbuffer = (LPSTR)GlobalLock(hbuffer);

    if (!lpbuffer) 
	{
        wmm_close();
     	return -1;
    }

    /* Set wave header */
    memset(&wavehdr, 0, sizeof wavehdr);
    wavehdr.lpData = lpbuffer;
    wavehdr.dwBufferLength = buffer_size;
    wavehdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP | WHDR_DONE;
    wavehdr.dwLoops = 0x7fffffff;

    /* Make a timer callback for clearing the buffer */
    if (timeGetDevCaps(&wmm_tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
	{
	    wmm_close();
     	return -1;
    }

    timeBeginPeriod(wmm_tc.wPeriodMin);
    beginperiod = 1;
    wmm_timer_id = timeSetEvent((*fragsize * 1000) / (*speed), 0, wmm_timercallback, 0, TIME_PERIODIC);
    if (!wmm_timer_id) 
	{
        wmm_close();
        return -1;
    }

    sndinitted = 1;
    return 0;
}


/* ------------------------------------------------------------------------
 * WMM Close
 */
void wmm_close(void)
{
    sndinitted = 0;

    if (wmm_timer_id) 
	{
        timeKillEvent(wmm_timer_id);
        wmm_timer_id = 0;
    }
    if (beginperiod) 
	{
        timeEndPeriod(wmm_tc.wPeriodMin);
        beginperiod = 0;
    }
    if (headerprepared) 
	{
        waveOutReset(hwaveout);
        waveOutUnprepareHeader(hwaveout, &wavehdr, sizeof(WAVEHDR));
        headerprepared = 0;
    }
    if (hwaveout) 
	{
        waveOutClose(hwaveout);
        hwaveout = NULL;
    }
    if (lpbuffer) 
	{
        GlobalUnlock(hbuffer);
        lpbuffer = NULL;
    }
    if (hbuffer) 
	{
        GlobalFree(hbuffer);
        hbuffer = NULL;
    }
}


/* ------------------------------------------------------------------------
 * WMM Bufferspace
 */
int wmm_bufferspace(void)
{
    DWORD play_cursor;
    int value;

    if (!sndinitted)
        return 0;

    if (wavehdr.dwFlags & WHDR_DONE)
	{
        return 0; /* Buffer stopped playing */
	}

    if (waveOutGetPosition(hwaveout, &mmtime, sizeof(mmtime)) != MMSYSERR_NOERROR )
	{
      return 0;
	}

    play_cursor = mmtime.u.cb;

	/* Take care of buffer wrap and possible counter rollover */
    play_cursor -= play_cursor_subtract;
    if (play_cursor >= buffer_size) 
	{
        play_cursor_subtract += (play_cursor / buffer_size)*buffer_size;
        play_cursor %= buffer_size;
    }

    /* Offset cursor by a magic value */
    play_cursor += play_cursor_offset;
    play_cursor %= buffer_size;

    value = write_cursor - play_cursor;

    if (value < 0)
        value += buffer_size;

    value = buffer_size - value;

    if (is16bit)
        value >>= 1;

    return value / num_of_channels;
}


/* ------------------------------------------------------------------------
 * WMM Write
 */
int wmm_write(SWORD *pbuf, size_t nr)
{
    DWORD play_cursor;
    DWORD worktodo;
    int t;
    char *destptr;
    int i;

    if (!sndinitted) return 0;


    /* Has buffer stopped playing? (or not yet started) */
    if (wavehdr.dwFlags & WHDR_DONE) 
	{
        waveOutReset(hwaveout); /* To clear position counter */
        if (headerprepared) 
		{
     	    waveOutUnprepareHeader(hwaveout, &wavehdr, sizeof(WAVEHDR));
            headerprepared = 0;
        }

        /* Clear buffer to start from silence */
        if (is16bit)
            memset(lpbuffer, 0, buffer_size);
        else
            memset(lpbuffer, 0x80, buffer_size);

        /* Reset writing pos, wrapping subtract, inactivity timer */
        write_cursor = buffer_size - fragment_bytesize;
        play_cursor_subtract = 0;
        inactivity_timer = 0;

        /* Set buffer looping */
        wavehdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        wavehdr.dwLoops = 0x7fffffff;
     	waveOutPrepareHeader(hwaveout, &wavehdr, sizeof(WAVEHDR));

        /* Start buffer playing */
        if (waveOutWrite(hwaveout, &wavehdr, sizeof wavehdr) != MMSYSERR_NOERROR) 
		{
            wmm_close();
            return -1;
        }
        headerprepared = 1;
    } 
	else
	{
        inactivity_timer = 0; /* Else, just reset inactivity timer */
	}

	worktodo = (DWORD)nr * (is16bit ? sizeof(SWORD) : 1);

    if (worktodo > buffer_size)
        return 0; /* Sanity check */

    for (;;) 
	{
        DWORD freebufspace;

        if (waveOutGetPosition(hwaveout, &mmtime, sizeof(mmtime)) != MMSYSERR_NOERROR)
		{
            return 0;
		}
        play_cursor = mmtime.u.cb;

        /* Take care of buffer wrap and possible counter rollover */
        play_cursor -= play_cursor_subtract;
        if (play_cursor >= buffer_size) 
		{
            play_cursor_subtract += (play_cursor / buffer_size)*buffer_size;
            play_cursor %= buffer_size;
        }

        /* Offset cursor by a magic value */
        play_cursor += play_cursor_offset;
        play_cursor %= buffer_size;

        /* Wait until enough free space in the circular buffer */
        freebufspace = (play_cursor - write_cursor);
        if (freebufspace >= worktodo)
            break;
        /* Also break out of loop if buffer stops playing */
        if (wavehdr.dwFlags & WHDR_DONE)
            break;
    }

    destptr = lpbuffer + write_cursor;

    if (is16bit) 
	{
        DWORD workend = write_cursor + worktodo;

        if (workend > buffer_size) 
		{
            DWORD part2 = workend - buffer_size;
            DWORD part1 = worktodo - part2;

            memcpy(destptr,pbuf,part1);
            pbuf += part1 >> 1;
            memcpy(lpbuffer,pbuf,part2);
            pbuf += part2 >> 1;
        }
		else
		{
            memcpy(destptr,pbuf,worktodo);
            pbuf += worktodo >> 1;
        }
    }
	else
	{
        for (t = 0; t < (int)worktodo; t++) 
		{
            *destptr++ = (*pbuf >> 8) + 0x80;
            if (destptr >= (lpbuffer + buffer_size))
                destptr = lpbuffer;
            pbuf++;
        }
    }

    pbuf -= num_of_channels;
    for (i = 0; i < num_of_channels; i++) 
	{
        last_buffered_sample[i] = *pbuf++;
    }

    /* Increment writing pos */
    write_cursor += worktodo;
    write_cursor %= buffer_size;
    return 0;
}


/* ------------------------------------------------------------------------
 * WMM Suspend
 */
int wmm_suspend(void)
{
    int c, i;
    SWORD *p;

    p = (SWORD*)malloc(fragment_size * num_of_channels * sizeof(SWORD));
	memset(p, 0, fragment_size * num_of_channels * sizeof(SWORD));

    if (!p)
        return 0;

    for (c = 0; c < num_of_channels; c++) 
	{
        for (i = 0; i < fragment_size; i++) 
            p[i * num_of_channels + c] = last_buffered_sample[c];
    }
    
    for (i = 0; i < num_fragments; i++)
        wmm_write(p, fragment_size * num_of_channels);

    free(p);
    return 0;
}


/* ------------------------------------------------------------------------
 * WMM Resume
 */
int wmm_resume(void)
{
    DWORD play_cursor;

    if (waveOutGetPosition(hwaveout, &mmtime, sizeof(mmtime)) != MMSYSERR_NOERROR)
      	return 0;

    play_cursor = mmtime.u.cb - (mmtime.u.cb % fragment_bytesize);

    write_cursor = play_cursor - fragment_bytesize;
    write_cursor %= buffer_size;
    play_cursor_subtract = 0;
    return 0;
}
