#ifndef ARMOR8CONSTANTS_HPP
#define ARMOR8CONSTANTS_HPP

#include <limits>

constexpr unsigned int ARMOR8_MIN_MIDI_VELOCITY = 30; // the lowest midi velocity to trigger a key press

// Operator Thresholds (X means times, so 6X means 6 times the original; D means divided by)
constexpr float OP_THRESHOLD_6X = 15000.0f;
constexpr float OP_THRESHOLD_5X = 10000.0f;
constexpr float OP_THRESHOLD_4X = 5000.0f;
constexpr float OP_THRESHOLD_3X = 2500.0f;
constexpr float OP_THRESHOLD_2X = 1500.0f;
constexpr float OP_THRESHOLD_1X = 1000.0f;
constexpr float OP_THRESHOLD_D2 = 500.0f;
constexpr float OP_THRESHOLD_D4 = std::numeric_limits<float>::min();

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

constexpr unsigned int ARMOR8_POT_STABIL_NUM = 50; // pot stabilization stuff
constexpr float ARMOR8_POT_MENU_CHANGE_THRESH = 0.5f; // threshold to break for a status submenu change

constexpr unsigned int ARMOR8_PARAMETER_EVENT_QUEUE_SIZE = 1000;
constexpr unsigned int ARMOR8_PRESET_EVENT_QUEUE_SIZE = 10;

constexpr unsigned int ARMOR8_NUM_OPERATORS_PER_VOICE = 4;

enum class PARAM_CHANNEL : unsigned int
{
	FREQUENCY 	= 0,
	DETUNE 		= 1,
	EG_ATTACK 	= 2,
	EG_DECAY 	= 3,
	EG_SUSTAIN 	= 4,
	EG_RELEASE 	= 5,
	EG_ATTACK_EXPO 	= 6,
	EG_DECAY_EXPO 	= 7,
	EG_RELEASE_EXPO = 8,
	OP_1_MOD_AMOUNT = 9,
	OP_2_MOD_AMOUNT = 10,
	OP_3_MOD_AMOUNT = 11,
	OP_4_MOD_AMOUNT = 12,
	AMPLITUDE 	= 13,
	FILTER_FREQ 	= 14,
	FILTER_RES 	= 15,
	AMP_VEL_SENS 	= 16,
	FILT_VEL_SENS 	= 17,
	PITCH_BEND_SEMI = 18,
	GLIDE_TIME 	= 19,
	USE_RATIO 	= 20,
	EG_DEST_AMP 	= 21,
	EG_DEST_FREQ 	= 22,
	EG_DEST_FILT 	= 23,
	GLIDE_RETRIG 	= 24,
	MONOPHONIC 	= 25,
	SELECT_WAVEFORM = 26,
	SELECT_OPERATOR = 27,
	NEXT_PRESET 	= 28,
	PREV_PRESET 	= 29,
	WRITE_PRESET 	= 30,
	POT1_ASSIGNMENT = 31,
	POT2_ASSIGNMENT = 32,
	POT3_ASSIGNMENT = 33,
};

enum class POT_CHANNEL : unsigned int
{
	EFFECT1 = 0,
	EFFECT2 = 1,
	EFFECT3 = 2
};

enum class BUTTON_CHANNEL : unsigned int
{
	EFFECT1 = 0,
	EFFECT2 = 1
};

#endif // ARMOR8CONSTANTS_HPP
