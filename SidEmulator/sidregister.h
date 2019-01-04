//  ---------------------------------------------------------------------------
// SID register definition file
//  ---------------------------------------------------------------------------

#ifndef __SIDREGISTER_H__
#define __SIDREGISTER_H__

#define SIDREG_ATTACK_MASK   (0xF0)
#define SIDREG_ATTACK_RANGE  (0x0F)
#define SIDREG_DECAY_MASK    (0x0F)
#define SIDREG_DECAY_RANGE   (0x0F)
#define SIDREG_SUSTAIN_MASK  (0xF0)
#define SIDREG_SUSTAIN_RANGE (0x0F)
#define SIDREG_RELEASE_MASK  (0x0F)
#define SIDREG_RELEASE_RANGE (0x0F)

typedef enum tSidRegister_en_t
{
  SIDREG_FREQ_LO_1,
  SIDREG_FREQ_HI_1,
  SIDREG_PW_LO_1,
  SIDREG_PW_HI_1,
  SIDREG_CONTROL_REG_1,
  SIDREG_ATTACK_DECAY_1,
  SIDREG_SUSTAIN_RELEASE_1,
  SIDREG_FREQ_LO_2,
  SIDREG_FREQ_HI_2,
  SIDREG_PW_LO_2,
  SIDREG_PW_HI_2,
  SIDREG_CONTROL_REG_2,
  SIDREG_ATTACK_DECAY_2,
  SIDREG_SUSTAIN_RELEASE_2,
  SIDREG_FREQ_LO_3,
  SIDREG_FREQ_HI_3,
  SIDREG_PW_LO_3,
  SIDREG_PW_HI_3,
  SIDREG_CONTROL_REG_3,
  SIDREG_ATTACK_DECAY_3,
  SIDREG_SUSTAIN_RELEASE_3,
  SIDREG_FC_LO,
  SIDREG_FC_HI,
  SIDREG_RES_FILT,
  SIDREG_MODE_VOL,
  SIDREG_POTX,
  SIDREG_POTY,
  SIDREG_POTX_OSC,
  SIDREG_POTX_ENV,
} tSidRegister_en;

typedef enum tSidControlBits_en_t
{
  SIDCTRL_GATE		= 0x01,
  SIDCTRL_SYNC		= 0x02,
  SIDCTRL_RINGMOD	= 0x04,
  SIDCTRL_TEST		= 0x08,
  SIDCTRL_TRIANGLE	= 0x10,
  SIDCTRL_SAWTOOTH	= 0x20,
  SIDCTRL_PULSE		= 0x40,
  SIDCTRL_NOISE		= 0x80,
} tSidControlBits_en;

typedef enum tSidFilterMode_t
{
  SIDFILTER_MODE_LP			= 0x10,
  SIDFILTER_MODE_BP			= 0x20,
  SIDFILTER_MODE_HP			= 0x30,
  SIDFILTER_MODE_VOICE3_OFF = 0x80,
} tSidFilterMode;

typedef enum tSidFilterSelection_t
{
  SIDFILTER_VOICE1 = 0x01,
  SIDFILTER_VOICE2 = 0x02,
  SIDFILTER_VOICE3 = 0x04,
  SIDFILTER_EXT    = 0x80,
} tSidFilterSelection;

#endif // not __SIDREGISTER_H__
