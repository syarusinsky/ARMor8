#ifndef ARMOR8UIMANAGER_HPP
#define ARMOR8UIMANAGER_HPP

/*************************************************************************
 * The ARMor8UiManager is a SIGL Surface that receives events when
 * the ARMor8VoiceManager changes a parameter and refreshes the frame
 * buffer accordingly. In order to be efficient as possible it also
 * sends screen refresh events indicating the part of the screen that
 * needs to be refreshed. This way the actual lcd HAL can send only the
 * relevant data, instead of an entire frame buffer.
*************************************************************************/

#include "Surface.hpp"

class Font;
class Sprite;

class ARMor8UiManager : public Surface
{
	public:
		ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format);
		~ARMor8UiManager() override;

		void setFont (Font* font);
		void setLogo (Sprite* logo);

		void draw() override;

	private:
		Sprite* m_Logo;
};

#endif // ARMOR8UIMANAGER_HPP
