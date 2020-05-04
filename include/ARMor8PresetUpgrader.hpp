#ifndef ARMOR8PRESETUPGRADER_HPP
#define ARMOR8PRESETUPGRADER_HPP

/*************************************************************************
 * The ARMor8PresetUpgrader is an IPresetUpgrader that is meant to be
 * passed to the PresetManager's upgradePresets function. It holds a
 * record of each version of the ARMor8VoiceState preset struct in its
 * cpp file for comparisons when upgrading.
*************************************************************************/

#include "PresetManager.hpp"

#include "ARMor8Voice.hpp"

class ARMor8PresetUpgrader : public IPresetUpgrader
{
	public:
		ARMor8PresetUpgrader(const ARMor8VoiceState& initPreset, const ARMor8PresetHeader& currentPresetHeader);
		~ARMor8PresetUpgrader() override;

		void upgradePresets() override;

	private:
		ARMor8VoiceState   m_InitPreset;
		ARMor8PresetHeader m_CurrentPresetHeader;

		void upgradeFrom0_1_0To1_0_0();
		void upgradeFrom1_0_0To1_1_0();
};

#endif // ARMOR8PRESETUPGRADER_HPP
