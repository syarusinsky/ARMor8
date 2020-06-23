#ifndef ARMOR8PRESETEVENTLISTENER_HPP
#define ARMOR8PRESETEVENTLISTENER_HPP

/*******************************************************************
 * An ARMor8PresetEventListener specifies a simple interface which
 * a subclass can use to be notified of ARMor8 preset events.
 * Specifically this means changing between operators and presets.
*******************************************************************/

#include "ARMor8Voice.hpp"
#include "IEventListener.hpp"

class ARMor8PresetEvent : public IEvent
{
	public:
		ARMor8PresetEvent (const ARMor8VoiceState& preset, unsigned int opToEdit, unsigned int presetNum,
					unsigned int channel);
		~ARMor8PresetEvent() override;

		ARMor8VoiceState getPreset() const { return m_Preset; }
		unsigned int getOpToEdit() const { return m_OpToEdit; }
		unsigned int getPresetNum() const { return m_PresetNum; }

	private:
		ARMor8VoiceState m_Preset;
		unsigned int m_OpToEdit;
		unsigned int m_PresetNum;
};

class IARMor8PresetEventListener : public IEventListener
{
	public:
		virtual ~IARMor8PresetEventListener();

		virtual void onARMor8PresetChangedEvent (const ARMor8PresetEvent& preset) = 0;

		void bindToARMor8PresetEventSystem();
		void unbindFromARMor8PresetEventSystem();

		static void PublishEvent (const ARMor8PresetEvent& preset);

	private:
		static EventDispatcher<IARMor8PresetEventListener, ARMor8PresetEvent,
					&IARMor8PresetEventListener::onARMor8PresetChangedEvent> m_EventDispatcher;
};

#endif // ARMOR8PRESETLISTENER_HPP
