#include "ARMor8PresetUpgrader.hpp"

struct ARMor8VoiceState_VERSION_0_1_0
{
	// operator 1
	float frequency1;
	bool  useRatio1;
	OscillatorMode wave1;
	float attack1;
	float attackExpo1;
	float decay1;
	float decayExpo1;
	float sustain1;
	float release1;
	float releaseExpo1;
	bool egAmplitudeMod1;
	bool egFrequencyMod1;
	bool egFilterMod1;
	float op1ModAmount1;
	float op2ModAmount1;
	float op3ModAmount1;
	float op4ModAmount1;
	float amplitude1;
	float filterFreq1;
	float filterRes1;
	float ampVelSens1;
	float filtVelSens1;

	// operator 2
	float frequency2;
	bool  useRatio2;
	OscillatorMode wave2;
	float attack2;
	float attackExpo2;
	float decay2;
	float decayExpo2;
	float sustain2;
	float release2;
	float releaseExpo2;
	bool egAmplitudeMod2;
	bool egFrequencyMod2;
	bool egFilterMod2;
	float op1ModAmount2;
	float op2ModAmount2;
	float op3ModAmount2;
	float op4ModAmount2;
	float amplitude2;
	float filterFreq2;
	float filterRes2;
	float ampVelSens2;
	float filtVelSens2;

	// operator 3
	float frequency3;
	bool  useRatio3;
	OscillatorMode wave3;
	float attack3;
	float attackExpo3;
	float decay3;
	float decayExpo3;
	float sustain3;
	float release3;
	float releaseExpo3;
	bool egAmplitudeMod3;
	bool egFrequencyMod3;
	bool egFilterMod3;
	float op1ModAmount3;
	float op2ModAmount3;
	float op3ModAmount3;
	float op4ModAmount3;
	float amplitude3;
	float filterFreq3;
	float filterRes3;
	float ampVelSens3;
	float filtVelSens3;

	// operator 4
	float frequency4;
	bool  useRatio4;
	OscillatorMode wave4;
	float attack4;
	float attackExpo4;
	float decay4;
	float decayExpo4;
	float sustain4;
	float release4;
	float releaseExpo4;
	bool egAmplitudeMod4;
	bool egFrequencyMod4;
	bool egFilterMod4;
	float op1ModAmount4;
	float op2ModAmount4;
	float op3ModAmount4;
	float op4ModAmount4;
	float amplitude4;
	float filterFreq4;
	float filterRes4;
	float ampVelSens4;
	float filtVelSens4;
};

struct ARMor8VoiceState_VERSION_1_0_0
{
	// operator 1
	float frequency1;
	bool  useRatio1;
	OscillatorMode wave1;
	float attack1;
	float attackExpo1;
	float decay1;
	float decayExpo1;
	float sustain1;
	float release1;
	float releaseExpo1;
	bool egAmplitudeMod1;
	bool egFrequencyMod1;
	bool egFilterMod1;
	float op1ModAmount1;
	float op2ModAmount1;
	float op3ModAmount1;
	float op4ModAmount1;
	float amplitude1;
	float filterFreq1;
	float filterRes1;
	float ampVelSens1;
	float filtVelSens1;

	// operator 2
	float frequency2;
	bool  useRatio2;
	OscillatorMode wave2;
	float attack2;
	float attackExpo2;
	float decay2;
	float decayExpo2;
	float sustain2;
	float release2;
	float releaseExpo2;
	bool egAmplitudeMod2;
	bool egFrequencyMod2;
	bool egFilterMod2;
	float op1ModAmount2;
	float op2ModAmount2;
	float op3ModAmount2;
	float op4ModAmount2;
	float amplitude2;
	float filterFreq2;
	float filterRes2;
	float ampVelSens2;
	float filtVelSens2;

	// operator 3
	float frequency3;
	bool  useRatio3;
	OscillatorMode wave3;
	float attack3;
	float attackExpo3;
	float decay3;
	float decayExpo3;
	float sustain3;
	float release3;
	float releaseExpo3;
	bool egAmplitudeMod3;
	bool egFrequencyMod3;
	bool egFilterMod3;
	float op1ModAmount3;
	float op2ModAmount3;
	float op3ModAmount3;
	float op4ModAmount3;
	float amplitude3;
	float filterFreq3;
	float filterRes3;
	float ampVelSens3;
	float filtVelSens3;

	// operator 4
	float frequency4;
	bool  useRatio4;
	OscillatorMode wave4;
	float attack4;
	float attackExpo4;
	float decay4;
	float decayExpo4;
	float sustain4;
	float release4;
	float releaseExpo4;
	bool egAmplitudeMod4;
	bool egFrequencyMod4;
	bool egFilterMod4;
	float op1ModAmount4;
	float op2ModAmount4;
	float op3ModAmount4;
	float op4ModAmount4;
	float amplitude4;
	float filterFreq4;
	float filterRes4;
	float ampVelSens4;
	float filtVelSens4;

	// global
	bool         monophonic;
	unsigned int pitchBendSemitones;
	float        glideTime;
	bool         glideRetrigger;
};

struct ARMor8VoiceState_VERSION_1_1_0
{
	// operator 1
	float frequency1;
	bool  useRatio1;
	OscillatorMode wave1;
	float attack1;
	float attackExpo1;
	float decay1;
	float decayExpo1;
	float sustain1;
	float release1;
	float releaseExpo1;
	bool egAmplitudeMod1;
	bool egFrequencyMod1;
	bool egFilterMod1;
	float op1ModAmount1;
	float op2ModAmount1;
	float op3ModAmount1;
	float op4ModAmount1;
	float amplitude1;
	float filterFreq1;
	float filterRes1;
	float ampVelSens1;
	float filtVelSens1;
	int detune1;

	// operator 2
	float frequency2;
	bool  useRatio2;
	OscillatorMode wave2;
	float attack2;
	float attackExpo2;
	float decay2;
	float decayExpo2;
	float sustain2;
	float release2;
	float releaseExpo2;
	bool egAmplitudeMod2;
	bool egFrequencyMod2;
	bool egFilterMod2;
	float op1ModAmount2;
	float op2ModAmount2;
	float op3ModAmount2;
	float op4ModAmount2;
	float amplitude2;
	float filterFreq2;
	float filterRes2;
	float ampVelSens2;
	float filtVelSens2;
	int detune2;

	// operator 3
	float frequency3;
	bool  useRatio3;
	OscillatorMode wave3;
	float attack3;
	float attackExpo3;
	float decay3;
	float decayExpo3;
	float sustain3;
	float release3;
	float releaseExpo3;
	bool egAmplitudeMod3;
	bool egFrequencyMod3;
	bool egFilterMod3;
	float op1ModAmount3;
	float op2ModAmount3;
	float op3ModAmount3;
	float op4ModAmount3;
	float amplitude3;
	float filterFreq3;
	float filterRes3;
	float ampVelSens3;
	float filtVelSens3;
	int detune3;

	// operator 4
	float frequency4;
	bool  useRatio4;
	OscillatorMode wave4;
	float attack4;
	float attackExpo4;
	float decay4;
	float decayExpo4;
	float sustain4;
	float release4;
	float releaseExpo4;
	bool egAmplitudeMod4;
	bool egFrequencyMod4;
	bool egFilterMod4;
	float op1ModAmount4;
	float op2ModAmount4;
	float op3ModAmount4;
	float op4ModAmount4;
	float amplitude4;
	float filterFreq4;
	float filterRes4;
	float ampVelSens4;
	float filtVelSens4;
	int detune4;

	// global
	bool         monophonic;
	unsigned int pitchBendSemitones;
	float        glideTime;
	bool         glideRetrigger;
};

struct ARMor8VoiceState_VERSION_1_2_0
{
	// operator 1
	float frequency1;
	bool  useRatio1;
	OscillatorMode wave1;
	float attack1;
	float attackExpo1;
	float decay1;
	float decayExpo1;
	float sustain1;
	float release1;
	float releaseExpo1;
	bool egAmplitudeMod1;
	bool egFrequencyMod1;
	bool egFilterMod1;
	float op1ModAmount1;
	float op2ModAmount1;
	float op3ModAmount1;
	float op4ModAmount1;
	float amplitude1;
	float filterFreq1;
	float filterRes1;
	float ampVelSens1;
	float filtVelSens1;
	int detune1;

	// operator 2
	float frequency2;
	bool  useRatio2;
	OscillatorMode wave2;
	float attack2;
	float attackExpo2;
	float decay2;
	float decayExpo2;
	float sustain2;
	float release2;
	float releaseExpo2;
	bool egAmplitudeMod2;
	bool egFrequencyMod2;
	bool egFilterMod2;
	float op1ModAmount2;
	float op2ModAmount2;
	float op3ModAmount2;
	float op4ModAmount2;
	float amplitude2;
	float filterFreq2;
	float filterRes2;
	float ampVelSens2;
	float filtVelSens2;
	int detune2;

	// operator 3
	float frequency3;
	bool  useRatio3;
	OscillatorMode wave3;
	float attack3;
	float attackExpo3;
	float decay3;
	float decayExpo3;
	float sustain3;
	float release3;
	float releaseExpo3;
	bool egAmplitudeMod3;
	bool egFrequencyMod3;
	bool egFilterMod3;
	float op1ModAmount3;
	float op2ModAmount3;
	float op3ModAmount3;
	float op4ModAmount3;
	float amplitude3;
	float filterFreq3;
	float filterRes3;
	float ampVelSens3;
	float filtVelSens3;
	int detune3;

	// operator 4
	float frequency4;
	bool  useRatio4;
	OscillatorMode wave4;
	float attack4;
	float attackExpo4;
	float decay4;
	float decayExpo4;
	float sustain4;
	float release4;
	float releaseExpo4;
	bool egAmplitudeMod4;
	bool egFrequencyMod4;
	bool egFilterMod4;
	float op1ModAmount4;
	float op2ModAmount4;
	float op3ModAmount4;
	float op4ModAmount4;
	float amplitude4;
	float filterFreq4;
	float filterRes4;
	float ampVelSens4;
	float filtVelSens4;
	int detune4;

	// global
	bool         monophonic;
	unsigned int pitchBendSemitones;
	float        glideTime;
	bool         glideRetrigger;
	unsigned int pot1AssignmentIndex;
	unsigned int pot1AssignmentOp;
	unsigned int pot2AssignmentIndex;
	unsigned int pot2AssignmentOp;
	unsigned int pot3AssignmentIndex;
	unsigned int pot3AssignmentOp;
};

ARMor8PresetUpgrader::ARMor8PresetUpgrader (const ARMor8VoiceState& initPreset, const ARMor8PresetHeader& currentPresetHeader) :
	m_InitPreset( initPreset ),
	m_CurrentPresetHeader( currentPresetHeader )
{
}

ARMor8PresetUpgrader::~ARMor8PresetUpgrader()
{
}

void ARMor8PresetUpgrader::upgradePresets()
{
	// first determine if the presets need to be initialized
	if ( m_PresetManager->needToInitializePresets() )
	{
		m_PresetManager->writeHeader<ARMor8PresetHeader>( m_CurrentPresetHeader );

		for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
		{
			m_PresetManager->writePreset<ARMor8VoiceState>( m_InitPreset, presetNum );
		}
	}
	else
	{
		// read the preset header from the storage media and determine the version
		ARMor8PresetHeader presetHeaderFromFile = m_PresetManager->retrieveHeader<ARMor8PresetHeader>();

		if ( presetHeaderFromFile.presetsFileInitialized == false ) // if set to reinitialize presets, reinitialize
		{
			m_PresetManager->writeHeader<ARMor8PresetHeader>( m_CurrentPresetHeader );

			for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
			{
				m_PresetManager->writePreset<ARMor8VoiceState>( m_InitPreset, presetNum );
			}
		}
		// if the versions don't match, we need to upgrade
		else if (       presetHeaderFromFile.versionMajor != m_CurrentPresetHeader.versionMajor ||
				presetHeaderFromFile.versionMinor != m_CurrentPresetHeader.versionMinor ||
				presetHeaderFromFile.versionPatch != m_CurrentPresetHeader.versionPatch )
		{
			if ( 		presetHeaderFromFile.versionMajor == 0 &&
					presetHeaderFromFile.versionMinor == 1 &&
					presetHeaderFromFile.versionPatch == 0 )
			{
				this->upgradeFrom0_1_0To1_0_0();
				this->upgradePresets(); // recursive in case it needs to upgrade from more than one version
			}
			else if (
					presetHeaderFromFile.versionMajor == 1 &&
					presetHeaderFromFile.versionMinor == 0 &&
					presetHeaderFromFile.versionPatch == 0 )
			{
				this->upgradeFrom1_0_0To1_1_0();
				this->upgradePresets(); // recursive in case it needs to upgrade from more than one version
			}
			else if (
					presetHeaderFromFile.versionMajor == 1 &&
					presetHeaderFromFile.versionMinor == 1 &&
					presetHeaderFromFile.versionPatch == 0 )
			{
				this->upgradeFrom1_1_0To1_2_0();
				this->upgradePresets(); // recursive in case it needs to upgrade from more than one version
			}
		}
	}
}

void ARMor8PresetUpgrader::upgradeFrom0_1_0To1_0_0()
{
	// write header
	ARMor8PresetHeader presetHeader = { 1, 0, 0, true };
	m_PresetManager->writeHeader<ARMor8PresetHeader>( presetHeader );

	// TODO this takes up a lottttt of memory, figure out a good way to manage this memory
	// store all old presets in an array, IStorageMedia and use SRAM???
	ARMor8VoiceState_VERSION_0_1_0 oldPresets[m_PresetManager->getMaxNumPresets()];

	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		oldPresets[presetNum] = m_PresetManager->retrievePreset<ARMor8VoiceState_VERSION_0_1_0>( presetNum );
	}

	// write new presets with default values for monophonic, glide time, glide retrigger, and pitch bend semitones
	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		ARMor8VoiceState_VERSION_1_0_0 newPreset =
		{
			// operator 1
			oldPresets[presetNum].frequency1,
			oldPresets[presetNum].useRatio1,
			oldPresets[presetNum].wave1,
			oldPresets[presetNum].attack1,
			oldPresets[presetNum].attackExpo1,
			oldPresets[presetNum].decay1,
			oldPresets[presetNum].decayExpo1,
			oldPresets[presetNum].sustain1,
			oldPresets[presetNum].release1,
			oldPresets[presetNum].releaseExpo1,
			oldPresets[presetNum].egAmplitudeMod1,
			oldPresets[presetNum].egFrequencyMod1,
			oldPresets[presetNum].egFilterMod1,
			oldPresets[presetNum].op1ModAmount1,
			oldPresets[presetNum].op2ModAmount1,
			oldPresets[presetNum].op3ModAmount1,
			oldPresets[presetNum].op4ModAmount1,
			oldPresets[presetNum].amplitude1,
			oldPresets[presetNum].filterFreq1,
			oldPresets[presetNum].filterRes1,
			oldPresets[presetNum].ampVelSens1,
			oldPresets[presetNum].filtVelSens1,

			// operator 2
			oldPresets[presetNum].frequency2,
			oldPresets[presetNum].useRatio2,
			oldPresets[presetNum].wave2,
			oldPresets[presetNum].attack2,
			oldPresets[presetNum].attackExpo2,
			oldPresets[presetNum].decay2,
			oldPresets[presetNum].decayExpo2,
			oldPresets[presetNum].sustain2,
			oldPresets[presetNum].release2,
			oldPresets[presetNum].releaseExpo2,
			oldPresets[presetNum].egAmplitudeMod2,
			oldPresets[presetNum].egFrequencyMod2,
			oldPresets[presetNum].egFilterMod2,
			oldPresets[presetNum].op1ModAmount2,
			oldPresets[presetNum].op2ModAmount2,
			oldPresets[presetNum].op3ModAmount2,
			oldPresets[presetNum].op4ModAmount2,
			oldPresets[presetNum].amplitude2,
			oldPresets[presetNum].filterFreq2,
			oldPresets[presetNum].filterRes2,
			oldPresets[presetNum].ampVelSens2,
			oldPresets[presetNum].filtVelSens2,

			// operator 3
			oldPresets[presetNum].frequency3,
			oldPresets[presetNum].useRatio3,
			oldPresets[presetNum].wave3,
			oldPresets[presetNum].attack3,
			oldPresets[presetNum].attackExpo3,
			oldPresets[presetNum].decay3,
			oldPresets[presetNum].decayExpo3,
			oldPresets[presetNum].sustain3,
			oldPresets[presetNum].release3,
			oldPresets[presetNum].releaseExpo3,
			oldPresets[presetNum].egAmplitudeMod3,
			oldPresets[presetNum].egFrequencyMod3,
			oldPresets[presetNum].egFilterMod3,
			oldPresets[presetNum].op1ModAmount3,
			oldPresets[presetNum].op2ModAmount3,
			oldPresets[presetNum].op3ModAmount3,
			oldPresets[presetNum].op4ModAmount3,
			oldPresets[presetNum].amplitude3,
			oldPresets[presetNum].filterFreq3,
			oldPresets[presetNum].filterRes3,
			oldPresets[presetNum].ampVelSens3,
			oldPresets[presetNum].filtVelSens3,

			// operator 4
			oldPresets[presetNum].frequency4,
			oldPresets[presetNum].useRatio4,
			oldPresets[presetNum].wave4,
			oldPresets[presetNum].attack4,
			oldPresets[presetNum].attackExpo4,
			oldPresets[presetNum].decay4,
			oldPresets[presetNum].decayExpo4,
			oldPresets[presetNum].sustain4,
			oldPresets[presetNum].release4,
			oldPresets[presetNum].releaseExpo4,
			oldPresets[presetNum].egAmplitudeMod4,
			oldPresets[presetNum].egFrequencyMod4,
			oldPresets[presetNum].egFilterMod4,
			oldPresets[presetNum].op1ModAmount4,
			oldPresets[presetNum].op2ModAmount4,
			oldPresets[presetNum].op3ModAmount4,
			oldPresets[presetNum].op4ModAmount4,
			oldPresets[presetNum].amplitude4,
			oldPresets[presetNum].filterFreq4,
			oldPresets[presetNum].filterRes4,
			oldPresets[presetNum].ampVelSens4,
			oldPresets[presetNum].filtVelSens4,

			// global
			false,
			1,
			0.0f,
			false
		};

		m_PresetManager->writePreset<ARMor8VoiceState_VERSION_1_0_0>( newPreset, presetNum );
	}
}

void ARMor8PresetUpgrader::upgradeFrom1_0_0To1_1_0()
{
	// write header
	ARMor8PresetHeader presetHeader = { 1, 1, 0, true };
	m_PresetManager->writeHeader<ARMor8PresetHeader>( presetHeader );

	// TODO this takes up a lottttt of memory, figure out a good way to manage this memory
	// store all old presets in an array, IStorageMedia and use SRAM???
	ARMor8VoiceState_VERSION_1_0_0 oldPresets[m_PresetManager->getMaxNumPresets()];

	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		oldPresets[presetNum] = m_PresetManager->retrievePreset<ARMor8VoiceState_VERSION_1_0_0>( presetNum );
	}

	// write new presets with default values for detune
	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		ARMor8VoiceState_VERSION_1_1_0 newPreset =
		{
			// operator 1
			oldPresets[presetNum].frequency1,
			oldPresets[presetNum].useRatio1,
			oldPresets[presetNum].wave1,
			oldPresets[presetNum].attack1,
			oldPresets[presetNum].attackExpo1,
			oldPresets[presetNum].decay1,
			oldPresets[presetNum].decayExpo1,
			oldPresets[presetNum].sustain1,
			oldPresets[presetNum].release1,
			oldPresets[presetNum].releaseExpo1,
			oldPresets[presetNum].egAmplitudeMod1,
			oldPresets[presetNum].egFrequencyMod1,
			oldPresets[presetNum].egFilterMod1,
			oldPresets[presetNum].op1ModAmount1,
			oldPresets[presetNum].op2ModAmount1,
			oldPresets[presetNum].op3ModAmount1,
			oldPresets[presetNum].op4ModAmount1,
			oldPresets[presetNum].amplitude1,
			oldPresets[presetNum].filterFreq1,
			oldPresets[presetNum].filterRes1,
			oldPresets[presetNum].ampVelSens1,
			oldPresets[presetNum].filtVelSens1,
			0,

			// operator 2
			oldPresets[presetNum].frequency2,
			oldPresets[presetNum].useRatio2,
			oldPresets[presetNum].wave2,
			oldPresets[presetNum].attack2,
			oldPresets[presetNum].attackExpo2,
			oldPresets[presetNum].decay2,
			oldPresets[presetNum].decayExpo2,
			oldPresets[presetNum].sustain2,
			oldPresets[presetNum].release2,
			oldPresets[presetNum].releaseExpo2,
			oldPresets[presetNum].egAmplitudeMod2,
			oldPresets[presetNum].egFrequencyMod2,
			oldPresets[presetNum].egFilterMod2,
			oldPresets[presetNum].op1ModAmount2,
			oldPresets[presetNum].op2ModAmount2,
			oldPresets[presetNum].op3ModAmount2,
			oldPresets[presetNum].op4ModAmount2,
			oldPresets[presetNum].amplitude2,
			oldPresets[presetNum].filterFreq2,
			oldPresets[presetNum].filterRes2,
			oldPresets[presetNum].ampVelSens2,
			oldPresets[presetNum].filtVelSens2,
			0,

			// operator 3
			oldPresets[presetNum].frequency3,
			oldPresets[presetNum].useRatio3,
			oldPresets[presetNum].wave3,
			oldPresets[presetNum].attack3,
			oldPresets[presetNum].attackExpo3,
			oldPresets[presetNum].decay3,
			oldPresets[presetNum].decayExpo3,
			oldPresets[presetNum].sustain3,
			oldPresets[presetNum].release3,
			oldPresets[presetNum].releaseExpo3,
			oldPresets[presetNum].egAmplitudeMod3,
			oldPresets[presetNum].egFrequencyMod3,
			oldPresets[presetNum].egFilterMod3,
			oldPresets[presetNum].op1ModAmount3,
			oldPresets[presetNum].op2ModAmount3,
			oldPresets[presetNum].op3ModAmount3,
			oldPresets[presetNum].op4ModAmount3,
			oldPresets[presetNum].amplitude3,
			oldPresets[presetNum].filterFreq3,
			oldPresets[presetNum].filterRes3,
			oldPresets[presetNum].ampVelSens3,
			oldPresets[presetNum].filtVelSens3,
			0,

			// operator 4
			oldPresets[presetNum].frequency4,
			oldPresets[presetNum].useRatio4,
			oldPresets[presetNum].wave4,
			oldPresets[presetNum].attack4,
			oldPresets[presetNum].attackExpo4,
			oldPresets[presetNum].decay4,
			oldPresets[presetNum].decayExpo4,
			oldPresets[presetNum].sustain4,
			oldPresets[presetNum].release4,
			oldPresets[presetNum].releaseExpo4,
			oldPresets[presetNum].egAmplitudeMod4,
			oldPresets[presetNum].egFrequencyMod4,
			oldPresets[presetNum].egFilterMod4,
			oldPresets[presetNum].op1ModAmount4,
			oldPresets[presetNum].op2ModAmount4,
			oldPresets[presetNum].op3ModAmount4,
			oldPresets[presetNum].op4ModAmount4,
			oldPresets[presetNum].amplitude4,
			oldPresets[presetNum].filterFreq4,
			oldPresets[presetNum].filterRes4,
			oldPresets[presetNum].ampVelSens4,
			oldPresets[presetNum].filtVelSens4,
			0,

			// global
			oldPresets[presetNum].monophonic,
			oldPresets[presetNum].pitchBendSemitones,
			oldPresets[presetNum].glideTime,
			oldPresets[presetNum].glideRetrigger
		};

		m_PresetManager->writePreset<ARMor8VoiceState_VERSION_1_1_0>( newPreset, presetNum );
	}
}

void ARMor8PresetUpgrader::upgradeFrom1_1_0To1_2_0()
{
	// write header
	ARMor8PresetHeader presetHeader = { 1, 2, 0, true };
	m_PresetManager->writeHeader<ARMor8PresetHeader>( presetHeader );

	// TODO this takes up a lottttt of memory, figure out a good way to manage this memory
	// store all old presets in an array, IStorageMedia and use SRAM???
	ARMor8VoiceState_VERSION_1_1_0 oldPresets[m_PresetManager->getMaxNumPresets()];

	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		oldPresets[presetNum] = m_PresetManager->retrievePreset<ARMor8VoiceState_VERSION_1_1_0>( presetNum );
	}

	// write new presets with default values for potentiometer assignments
	for (unsigned int presetNum = 0; presetNum < m_PresetManager->getMaxNumPresets(); presetNum++)
	{
		ARMor8VoiceState_VERSION_1_2_0 newPreset =
		{
			// operator 1
			oldPresets[presetNum].frequency1,
			oldPresets[presetNum].useRatio1,
			oldPresets[presetNum].wave1,
			oldPresets[presetNum].attack1,
			oldPresets[presetNum].attackExpo1,
			oldPresets[presetNum].decay1,
			oldPresets[presetNum].decayExpo1,
			oldPresets[presetNum].sustain1,
			oldPresets[presetNum].release1,
			oldPresets[presetNum].releaseExpo1,
			oldPresets[presetNum].egAmplitudeMod1,
			oldPresets[presetNum].egFrequencyMod1,
			oldPresets[presetNum].egFilterMod1,
			oldPresets[presetNum].op1ModAmount1,
			oldPresets[presetNum].op2ModAmount1,
			oldPresets[presetNum].op3ModAmount1,
			oldPresets[presetNum].op4ModAmount1,
			oldPresets[presetNum].amplitude1,
			oldPresets[presetNum].filterFreq1,
			oldPresets[presetNum].filterRes1,
			oldPresets[presetNum].ampVelSens1,
			oldPresets[presetNum].filtVelSens1,
			oldPresets[presetNum].detune1,

			// operator 2
			oldPresets[presetNum].frequency2,
			oldPresets[presetNum].useRatio2,
			oldPresets[presetNum].wave2,
			oldPresets[presetNum].attack2,
			oldPresets[presetNum].attackExpo2,
			oldPresets[presetNum].decay2,
			oldPresets[presetNum].decayExpo2,
			oldPresets[presetNum].sustain2,
			oldPresets[presetNum].release2,
			oldPresets[presetNum].releaseExpo2,
			oldPresets[presetNum].egAmplitudeMod2,
			oldPresets[presetNum].egFrequencyMod2,
			oldPresets[presetNum].egFilterMod2,
			oldPresets[presetNum].op1ModAmount2,
			oldPresets[presetNum].op2ModAmount2,
			oldPresets[presetNum].op3ModAmount2,
			oldPresets[presetNum].op4ModAmount2,
			oldPresets[presetNum].amplitude2,
			oldPresets[presetNum].filterFreq2,
			oldPresets[presetNum].filterRes2,
			oldPresets[presetNum].ampVelSens2,
			oldPresets[presetNum].filtVelSens2,
			oldPresets[presetNum].detune2,

			// operator 3
			oldPresets[presetNum].frequency3,
			oldPresets[presetNum].useRatio3,
			oldPresets[presetNum].wave3,
			oldPresets[presetNum].attack3,
			oldPresets[presetNum].attackExpo3,
			oldPresets[presetNum].decay3,
			oldPresets[presetNum].decayExpo3,
			oldPresets[presetNum].sustain3,
			oldPresets[presetNum].release3,
			oldPresets[presetNum].releaseExpo3,
			oldPresets[presetNum].egAmplitudeMod3,
			oldPresets[presetNum].egFrequencyMod3,
			oldPresets[presetNum].egFilterMod3,
			oldPresets[presetNum].op1ModAmount3,
			oldPresets[presetNum].op2ModAmount3,
			oldPresets[presetNum].op3ModAmount3,
			oldPresets[presetNum].op4ModAmount3,
			oldPresets[presetNum].amplitude3,
			oldPresets[presetNum].filterFreq3,
			oldPresets[presetNum].filterRes3,
			oldPresets[presetNum].ampVelSens3,
			oldPresets[presetNum].filtVelSens3,
			oldPresets[presetNum].detune3,

			// operator 4
			oldPresets[presetNum].frequency4,
			oldPresets[presetNum].useRatio4,
			oldPresets[presetNum].wave4,
			oldPresets[presetNum].attack4,
			oldPresets[presetNum].attackExpo4,
			oldPresets[presetNum].decay4,
			oldPresets[presetNum].decayExpo4,
			oldPresets[presetNum].sustain4,
			oldPresets[presetNum].release4,
			oldPresets[presetNum].releaseExpo4,
			oldPresets[presetNum].egAmplitudeMod4,
			oldPresets[presetNum].egFrequencyMod4,
			oldPresets[presetNum].egFilterMod4,
			oldPresets[presetNum].op1ModAmount4,
			oldPresets[presetNum].op2ModAmount4,
			oldPresets[presetNum].op3ModAmount4,
			oldPresets[presetNum].op4ModAmount4,
			oldPresets[presetNum].amplitude4,
			oldPresets[presetNum].filterFreq4,
			oldPresets[presetNum].filterRes4,
			oldPresets[presetNum].ampVelSens4,
			oldPresets[presetNum].filtVelSens4,
			oldPresets[presetNum].detune4,

			// global
			oldPresets[presetNum].monophonic,
			oldPresets[presetNum].pitchBendSemitones,
			oldPresets[presetNum].glideTime,
			oldPresets[presetNum].glideRetrigger,
			0,
			0,
			1,
			0,
			2,
			0
		};

		m_PresetManager->writePreset<ARMor8VoiceState_VERSION_1_2_0>( newPreset, presetNum );
	}
}
