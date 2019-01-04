//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 2004  Dag Lem <resid@nimrod.no>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>

#include "sid.h"
#include "sidregister.h"

/* VOID CALLBACK pfTimerCb( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  static int a;

  a++;

  return;
}


void main()
{
  SID      oSID;
  FILE    *pFile;
  errno_t  tErr;
  char     acErrBuf[80];

  oSID.reset();
  oSID.set_chip_model(MOS6581);

  tErr = fopen_s(&pFile, "sample.snd", "w");

  if( tErr != 0 ) 
  {
    printf("Cannot open file.\n");
	strerror_s(acErrBuf, 80, tErr);
	fprintf( stderr, acErrBuf );

    getchar();
    exit(1);
  }

  SetTimer(NULL,        // Window Handle
    1234,               // timer identifier 
    USER_TIMER_MINIMUM, // 10ms-second interval (minimum)
    pfTimerCb);         // timer callback

  oSID.write(SIDREG_FREQ_LO_1, 0xCD); // Note A0
  oSID.write(SIDREG_FREQ_HI_1, 0x01);
  oSID.write(SIDREG_CONTROL_REG_1, (SIDCTRL_TRIANGLE | SIDCTRL_GATE));
  oSID.write(SIDREG_ATTACK_DECAY_1, 0x66);
  oSID.write(SIDREG_SUSTAIN_RELEASE_1, 0x55);
  oSID.write(SIDREG_MODE_VOL, 0x0F);

  // while(1)
  {
    int iSample;

	oSID.clock();
	iSample = oSID.output(12);

	fwrite( (void*)&iSample, 2, 1, pFile );
  }
  getchar();
  return;
}
*/
