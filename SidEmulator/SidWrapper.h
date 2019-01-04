#include "siddefs.h"
#include "sid.h"

typedef struct SidWrapper_SIDregs_named_t
{
	reg8 u8SIDREG_FREQ_LO_1;
	reg8 u8SIDREG_FREQ_HI_1;
	reg8 u8SIDREG_PW_LO_1;
	reg8 u8SIDREG_PW_HI_1;
	reg8 u8SIDREG_CONTROL_REG_1;
	reg8 u8SIDREG_ATTACK_DECAY_1;
	reg8 u8SIDREG_SUSTAIN_RELEASE_1;
	reg8 u8SIDREG_FREQ_LO_2;
	reg8 u8SIDREG_FREQ_HI_2;
	reg8 u8SIDREG_PW_LO_2;
	reg8 u8SIDREG_PW_HI_2;
	reg8 u8SIDREG_CONTROL_REG_2;
	reg8 u8SIDREG_ATTACK_DECAY_2;
	reg8 u8SIDREG_SUSTAIN_RELEASE_2;
	reg8 u8SIDREG_FREQ_LO_3;
	reg8 u8SIDREG_FREQ_HI_3;
	reg8 u8SIDREG_PW_LO_3;
	reg8 u8SIDREG_PW_HI_3;
	reg8 u8SIDREG_CONTROL_REG_3;
	reg8 u8SIDREG_ATTACK_DECAY_3;
	reg8 u8SIDREG_SUSTAIN_RELEASE_3;
	reg8 u8SIDREG_FC_LO;
	reg8 u8SIDREG_FC_HI;
	reg8 u8SIDREG_RES_FILT;
	reg8 u8SIDREG_MODE_VOL;
	reg8 u8SIDREG_POTX;
	reg8 u8SIDREG_POTY;
	reg8 u8SIDREG_POTX_OSC;
	reg8 u8SIDREG_POTX_ENV;
} SidWrapper_SIDregs_named_st;


typedef union SidWrapper_SIDregs_t
{
	SidWrapper_SIDregs_named_st sSidWrapper_SIDregs_named;
	reg8						au8SidWrapper_SIDregs[sizeof(SidWrapper_SIDregs_named_st)];
} SidWrapper_SIDregs_st;


class SidWrapper : public C64_SID
{
	public :
	SidWrapper();
	~SidWrapper();

	reg8 read(reg8 u8Register);
	bool write(reg8 u8Register, reg8 u8Value);

	private :
	SidWrapper_SIDregs_st sSidWrapper_SIDregs;
};
