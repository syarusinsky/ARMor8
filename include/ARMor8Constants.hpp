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

constexpr unsigned int ARMOR8_POT_STABIL_NUM = 50; // pot stabilization stuff
constexpr float ARMOR8_POT_STABIL_ALLOWED_SCATTER = 0.0f; // allow 0% of jitter on pots

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
