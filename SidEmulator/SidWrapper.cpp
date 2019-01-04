#include <StdAfx.h>
#include "SidWrapper.h"
#include "sidregister.h"

#include <memory.h>


SidWrapper::SidWrapper()
{
	int iIdx;

	memset(&sSidWrapper_SIDregs, 0, sizeof(sSidWrapper_SIDregs));

	/* Initialize C64 SID */
	for(iIdx = 0; iIdx<=SIDREG_POTX_ENV; iIdx++)
	{
		C64_SID::write(iIdx, 0);
	}
}


SidWrapper::~SidWrapper()
{
	int iIdx;

	memset(&sSidWrapper_SIDregs, 0, sizeof(sSidWrapper_SIDregs));

	/* Initialize C64 SID */
	for(iIdx = 0; iIdx<=SIDREG_POTX_ENV; iIdx++)
	{
		C64_SID::write(iIdx, 0);
	}
}


reg8 SidWrapper::read(reg8 u8Register)
{
	reg8 u8Value;

	if( u8Register <= SIDREG_MODE_VOL )
	{	/* write only register */
		u8Value = sSidWrapper_SIDregs.au8SidWrapper_SIDregs[u8Register];
	}
	else
	{	/* read only register */
		u8Value = C64_SID::read(u8Register);
		sSidWrapper_SIDregs.au8SidWrapper_SIDregs[u8Register] = u8Value;
	}

	return u8Value;
}


bool SidWrapper::write(reg8 u8Register, reg8 u8Value)
{
	if(u8Register <= SIDREG_MODE_VOL)
	{
		sSidWrapper_SIDregs.au8SidWrapper_SIDregs[u8Register] = u8Value;
		C64_SID::write(u8Register, u8Value);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
 }
