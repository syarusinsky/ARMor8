#ifndef ARMOR8CONSTANTS_HPP
#define ARMOR8CONSTANTS_HPP

constexpr float ARMOR8_FREQUENCY_MIN  = 1.0f;
constexpr float ARMOR8_FREQUENCY_MAX  = 20000.0f;
constexpr int   ARMOR8_DETUNE_MIN     = -1200;
constexpr int   ARMOR8_DETUNE_MAX     = 1200;
constexpr float ARMOR8_EXPO_MIN       = 0.1f;
constexpr float ARMOR8_EXPO_MAX       = 100.0f;
constexpr float ARMOR8_ATTACK_MIN     = 0.002f;
constexpr float ARMOR8_ATTACK_MAX     = 2.0f;
constexpr float ARMOR8_DECAY_MIN      = 0.0f;
constexpr float ARMOR8_DECAY_MAX      = 2.0f;
constexpr float ARMOR8_SUSTAIN_MIN    = 0.0f;
constexpr float ARMOR8_SUSTAIN_MAX    = 1.0f;
constexpr float ARMOR8_RELEASE_MIN    = 0.002f;
constexpr float ARMOR8_RELEASE_MAX    = 3.0f;
constexpr float ARMOR8_OP_MOD_MIN     = 0.0f;
constexpr float ARMOR8_OP_MOD_MAX     = 20000.0f;
constexpr float ARMOR8_AMPLITUDE_MIN  = 0.0f;
constexpr float ARMOR8_AMPLITUDE_MAX  = 5.0f;
constexpr float ARMOR8_FILT_FREQ_MIN  = 1.0f;
constexpr float ARMOR8_FILT_FREQ_MAX  = 20000.0f;
constexpr float ARMOR8_FILT_RES_MIN   = 0.0f;
constexpr float ARMOR8_FILT_RES_MAX   = 3.5f;
constexpr float ARMOR8_VELOCITY_MIN   = 0.0f;
constexpr float ARMOR8_VELOCITY_MAX   = 1.0f;
constexpr int   ARMOR8_PITCH_BEND_MIN = 1;
constexpr int   ARMOR8_PITCH_BEND_MAX = 12;
constexpr float ARMOR8_GLIDE_TIME_MIN = 0.0f;
constexpr float ARMOR8_GLIDE_TIME_MAX = 1.0f;

enum class POT_CHANNEL : unsigned int
{
	ALL          = 0,
	FREQUENCY    = 1,
	DETUNE       = 2,
	ATTACK       = 3,
	ATTACK_EXPO  = 4,
	DECAY        = 5,
	DECAY_EXPO   = 6,
	SUSTAIN      = 7,
	RELEASE      = 8,
	RELEASE_EXPO = 9,
	OP1_MOD_AMT  = 10,
	OP2_MOD_AMT  = 11,
	OP3_MOD_AMT  = 12,
	OP4_MOD_AMT  = 13,
	AMPLITUDE    = 14,
	FILT_FREQ    = 15,
	FILT_RES     = 16,
	VEL_AMP      = 17,
	VEL_FILT     = 18,
	PITCH_BEND   = 19,
	GLIDE_TIME   = 20
};

enum class BUTTON_CHANNEL : unsigned int
{
	ALL          = 0,
	RATIO        = 1,
	OP1          = 2,
	OP2          = 3,
	OP3          = 4,
	OP4          = 5,
	SINE         = 6,
	TRIANGLE     = 7,
	SQUARE       = 8,
	SAWTOOTH     = 9,
	EG_AMP       = 10,
	EG_FREQ      = 11,
	EG_FILT      = 12,
	GLIDE_RETRIG = 13,
	MONOPHONIC   = 14,
	PREV_PRESET  = 15,
	NEXT_PRESET  = 16,
	WRITE_PRESET = 17
};

#endif // ARMOR8CONSTANTS_HPP
