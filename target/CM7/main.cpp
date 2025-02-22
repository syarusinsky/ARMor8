#include "../../lib/STM32h745zi-HAL/llpd/include/LLPD.hpp"

#include "EEPROM_CAT24C64.hpp"
#include "SRAM_23K256.hpp"
#include "SDCard.hpp"
#include "EventQueue.hpp"
#include "MidiHandler.hpp"
#include "ARMor8VoiceManager.hpp"
#include "ARMor8PresetUpgrader.hpp"
#include "AudioBuffer.hpp"
#include "AudioConstants.hpp"
#include "IARMor8PresetEventListener.hpp"

#define SYS_CLOCK_FREQUENCY = 480000000;

// global variables
MidiHandler* volatile midiHandlerPtr = nullptr;
AudioBuffer<float>* volatile audioBufferPtr = nullptr;
// because we use DTCM and the DMA controller can't access that, we use axi sram
constexpr unsigned int dmaAudioBufferSize = ABUFFER_SIZE * 2 * sizeof(uint16_t); // x2 for packed left/right
uint16_t* dmaAudioBuffer1 = (uint16_t*) D1_AXISRAM_BASE;
uint16_t* dmaAudioBuffer2 = (uint16_t*) ( D1_AXISRAM_BASE + dmaAudioBufferSize );

// peripheral defines
#define OP_AMP1_INV_OUT_PORT 		GPIO_PORT::C
#define OP_AMP1_NONINVR_PORT 		GPIO_PORT::B
#define OP_AMP1_INVERT_PIN 		GPIO_PIN::PIN_5
#define OP_AMP1_OUTPUT_PIN 		GPIO_PIN::PIN_4
#define OP_AMP1_NON_INVERT_PIN 		GPIO_PIN::PIN_0
#define OP_AMP2_PORT 			GPIO_PORT::E
#define OP_AMP2_INVERT_PIN 		GPIO_PIN::PIN_8
#define OP_AMP2_OUTPUT_PIN 		GPIO_PIN::PIN_7
#define OP_AMP2_NON_INVERT_PIN 		GPIO_PIN::PIN_9
#define EFFECT_ADC_PORT 		GPIO_PORT::A
#define EFFECT_ADC_NUM 			ADC_NUM::ADC_1_2
#define EFFECT1_ADC_PIN 		GPIO_PIN::PIN_2
#define EFFECT1_ADC_CHANNEL 		ADC_CHANNEL::CHAN_14
#define EFFECT2_ADC_PIN 		GPIO_PIN::PIN_3
#define EFFECT2_ADC_CHANNEL 		ADC_CHANNEL::CHAN_15
#define EFFECT3_ADC_PIN 		GPIO_PIN::PIN_0
#define EFFECT3_ADC_CHANNEL 		ADC_CHANNEL::CHAN_16
#define AUDIO_IN_PORT 			GPIO_PORT::C
#define AUDIO_IN_ADC_NUM 		ADC_NUM::ADC_3
#define AUDIO1_IN_PIN 			GPIO_PIN::PIN_2
#define AUDIO1_IN_ADC_CHANNEL 		ADC_CHANNEL::CHAN_0
#define AUDIO2_IN_PIN 			GPIO_PIN::PIN_3
#define AUDIO2_IN_ADC_CHANNEL 		ADC_CHANNEL::CHAN_1
#define EFFECT_BUTTON_PORT 		GPIO_PORT::E
#define EFFECT1_BUTTON_PIN 		GPIO_PIN::PIN_0
#define EFFECT2_BUTTON_PIN 		GPIO_PIN::PIN_1
#define SRAM_CS_PORT 			GPIO_PORT::D
#define SRAM1_CS_PIN 			GPIO_PIN::PIN_8
#define SRAM2_CS_PIN 			GPIO_PIN::PIN_9
#define SRAM3_CS_PIN 			GPIO_PIN::PIN_10
#define SRAM4_CS_PIN 			GPIO_PIN::PIN_11
#define EEPROM1_ADDRESS 		false, false, false
#define EEPROM2_ADDRESS 		true, false, false
#define EEPROM3_ADDRESS 		false, true, false
#define EEPROM4_ADDRESS 		true, true, false
#define SD_CARD_CS_PORT 		GPIO_PORT::E
#define SD_CARD_CS_PIN 			GPIO_PIN::PIN_15
#define OLED_PORT 			GPIO_PORT::C
#define OLED_RESET_PIN 			GPIO_PIN::PIN_13
#define OLED_DC_PIN 			GPIO_PIN::PIN_14
#define OLED_CS_PIN 			GPIO_PIN::PIN_11
#define MIDI_USART_NUM 			USART_NUM::USART_6
#define LOGGING_USART_NUM 		USART_NUM::USART_2
#define EEPROM_I2C_NUM 			I2C_NUM::I2C_1
#define SRAM_SPI_NUM 			SPI_NUM::SPI_2
#define SD_CARD_SPI_NUM 		SPI_NUM::SPI_4
#define OLED_SPI_NUM 			SPI_NUM::SPI_3

// this class is specifically to check if the eeprom has been initialized with the correct code at the end of the eeprom addresses
class Eeprom_CAT24C64_Manager_ARMor8 : public Eeprom_CAT24C64_Manager
{
	public:
		Eeprom_CAT24C64_Manager_ARMor8 (const I2C_NUM& i2cNum, const std::vector<Eeprom_CAT24C64_AddressConfig>& addressConfigs) :
			Eeprom_CAT24C64_Manager( i2cNum, addressConfigs ) {}

		bool needsInitialization() override
		{
			for ( unsigned int byte = 0; byte < sizeof(m_InitCode); byte++ )
			{
				uint8_t value = this->readByte( m_InitCodeStartAddress + byte );
				if ( value != m_InitCode[byte] )
				{
					// LLPD::usart_log( LOGGING_USART_NUM, "EEPROM init code not detected, initializing now..." );

					return true;
				}
			}

			// LLPD::usart_log( LOGGING_USART_NUM, "EEPROM init code detected, loading preset now..." );

			return false;
		}

		void afterInitialize() override
		{
			for ( unsigned int byte = 0; byte < sizeof(m_InitCode); byte++ )
			{
				this->writeByte( m_InitCodeStartAddress + byte, m_InitCode[byte] );
			}

			for ( unsigned int byte = 0; byte < sizeof(m_InitCode); byte++ )
			{
				uint8_t value = this->readByte( m_InitCodeStartAddress + byte );
				if ( value != m_InitCode[byte] )
				{
					// LLPD::usart_log( LOGGING_USART_NUM, "EEPROM failed to initialize, check connections and setup..." );

					return;
				}
			}

			// LLPD::usart_log( LOGGING_USART_NUM, "EEPROM initialized successfully, loading preset now..." );
		}

	private:
		uint8_t m_InitCode[8] = { 0b10010100, 0b11100010, 0b11001010, 0b00011110, 0b11001100, 0b01101001, 0b11110100, 0b01111110 };
		unsigned int m_InitCodeStartAddress = ( Eeprom_CAT24C64::EEPROM_SIZE * m_Eeproms.size() ) - sizeof( m_InitCode );
};

// this class is to retrieve parameter events from the m4 core via the parameter event queue and republish to the voice manager
class ARMor8ParameterEventBridge
{
	public:
		ARMor8ParameterEventBridge (EventQueue<ARMor8ParameterEvent>* eventQueuePtr) : m_EventQueuePtr( eventQueuePtr ) {}
		~ARMor8ParameterEventBridge() {}

		void processQueuedParameterEvents()
		{
			ARMor8ParameterEvent paramEvent( 0.0f, 0, 0 );
			bool readCorrectly = m_EventQueuePtr->readEvent( paramEvent );
			while ( readCorrectly )
			{
				IARMor8ParameterEventListener::PublishEvent( paramEvent );

				readCorrectly = m_EventQueuePtr->readEvent( paramEvent );
			}
		}

	private:
		EventQueue<ARMor8ParameterEvent>* m_EventQueuePtr;
};

// this class is to receive the published preset events from the voice manager and put them in the event queue for the m4 core to retrieve
class ARMor8PresetEventBridge : public IARMor8PresetEventListener
{
	public:
		ARMor8PresetEventBridge (EventQueue<ARMor8PresetEvent>* eventQueuePtr) : m_EventQueuePtr( eventQueuePtr ) {}
		~ARMor8PresetEventBridge() override {}

		void onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent) override
		{
			m_EventQueuePtr->writeEvent( presetEvent );
		}

	private:
		EventQueue<ARMor8PresetEvent>* m_EventQueuePtr;
};

// these pins are unused for ARMor8, so we disable them as per the ST recommendations
void disableUnusedPins()
{
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_7, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_12, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_0, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_2, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_4, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_12, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_13, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_14, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_0, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_2, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_3, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_4, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_5, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_0, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_2, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_3, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_4, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_5, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_7, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_12, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_13, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_14, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::D, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_2, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_3, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_4, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_5, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_7, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_12, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_13, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_14, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_7, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_14, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_15, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_7, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_9, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_12, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_13, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::G, GPIO_PIN::PIN_14, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( AUDIO_IN_PORT, AUDIO1_IN_PIN, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( AUDIO_IN_PORT, AUDIO2_IN_PIN, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
}

int main(void)
{
	// TODO maybe move this stuff as well as caching to the HAL?
	// disable mpu
	__DMB();
	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
	MPU->CTRL = 0;
	// region config for sram4 as non-cacheable
	MPU->RNR = 0;
	MPU->RBAR = D3_SRAM_BASE;
	MPU->RASR = 	0 			 	<< 	MPU_RASR_XN_Pos 	|
			ARM_MPU_AP_FULL 	 	<< 	MPU_RASR_AP_Pos 	|
			0 			 	<< 	MPU_RASR_TEX_Pos 	|
			0 			 	<< 	MPU_RASR_S_Pos 		|
			0 			 	<< 	MPU_RASR_C_Pos 		|
			0 			 	<< 	MPU_RASR_B_Pos 		|
			0 			 	<< 	MPU_RASR_SRD_Pos 	|
			ARM_MPU_REGION_SIZE_64KB 	<< 	MPU_RASR_SIZE_Pos 	|
			1 				<< 	MPU_RASR_ENABLE_Pos;
	// region config for axi sram as non-cacheable
	MPU->RNR = 1;
	MPU->RBAR = D1_AXISRAM_BASE;
	MPU->RASR = 	0 			 	<< 	MPU_RASR_XN_Pos 	|
			ARM_MPU_AP_FULL 	 	<< 	MPU_RASR_AP_Pos 	|
			0 			 	<< 	MPU_RASR_TEX_Pos 	|
			0 			 	<< 	MPU_RASR_S_Pos 		|
			0 			 	<< 	MPU_RASR_C_Pos 		|
			0 			 	<< 	MPU_RASR_B_Pos 		|
			0 			 	<< 	MPU_RASR_SRD_Pos 	|
			ARM_MPU_REGION_SIZE_64KB 	<< 	MPU_RASR_SIZE_Pos 	|
			1 				<< 	MPU_RASR_ENABLE_Pos;
	// enable mpu
	MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	__DSB();
	__ISB();

	// setup clock 480MHz (also prescales peripheral clocks to fit rate limitations)
	LLPD::rcc_clock_start_max_cpu1();

	// enable gpio clocks
	LLPD::gpio_enable_clock( GPIO_PORT::A );
	LLPD::gpio_enable_clock( GPIO_PORT::B );
	LLPD::gpio_enable_clock( GPIO_PORT::C );
	LLPD::gpio_enable_clock( GPIO_PORT::D );
	LLPD::gpio_enable_clock( GPIO_PORT::E );
	LLPD::gpio_enable_clock( GPIO_PORT::F );
	LLPD::gpio_enable_clock( GPIO_PORT::G );
	LLPD::gpio_enable_clock( GPIO_PORT::H );

	// disable unused pins
	disableUnusedPins();

	// USART setup
	// LLPD::usart_init( LOGGING_USART_NUM, USART_WORD_LENGTH::BITS_8, USART_PARITY::NONE, USART_CONF::TX_AND_RX,
	// 			USART_STOP_BITS::BITS_1, 120000000, 9600 );
	// LLPD::usart_log( LOGGING_USART_NUM, "Ultra_FX_SYN starting up ----------------------------" );
	LLPD::usart_init( MIDI_USART_NUM, USART_WORD_LENGTH::BITS_8, USART_PARITY::NONE, USART_CONF::TX_AND_RX,
					USART_STOP_BITS::BITS_1, 120000000, 31250 );

	// audio timer setup (for 30 kHz sampling rate at 480 MHz / 2 timer clock)
	LLPD::tim6_counter_setup( 0, 8000, 30000 );
	// LLPD::tim6_counter_enable_interrupts(); // you need this for interrupt-based audio
	// LLPD::usart_log( LOGGING_USART_NUM, "tim6 initialized..." );

	// zero-out the dma audio buffers
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		uint16_t sampleVal = 4095 / 2;
		// left/right packed
		dmaAudioBuffer1[(sample * 2) + 0] = sampleVal;
		dmaAudioBuffer1[(sample * 2) + 1] = sampleVal;
		dmaAudioBuffer2[(sample * 2) + 0] = sampleVal;
		dmaAudioBuffer2[(sample * 2) + 1] = sampleVal;
	}

	// DAC setup
	// LLPD::dac_init( true ); // for interrupt-based audio
	LLPD::dac_init_use_dma( true, (uint32_t*) dmaAudioBuffer1, (uint32_t*) dmaAudioBuffer2, ABUFFER_SIZE );
	// LLPD::usart_log( LOGGING_USART_NUM, "dac initialized..." );

	// spi initialization
	LLPD::spi_master_init( OLED_SPI_NUM, SPI_BAUD_RATE::SYSCLK_DIV_BY_256, SPI_CLK_POL::LOW_IDLE, SPI_CLK_PHASE::FIRST,
				SPI_DUPLEX::FULL, SPI_FRAME_FORMAT::MSB_FIRST, SPI_DATA_SIZE::BITS_8 );
	// LLPD::usart_log( LOGGING_USART_NUM, "spi initialized..." );

	// i2c initialization
	LLPD::i2c_master_setup( EEPROM_I2C_NUM, 0x308075AE );
	// LLPD::usart_log( LOGGING_USART_NUM, "i2c initialized..." );

	// audio timer start
	LLPD::tim6_counter_start();
	// LLPD::usart_log( LOGGING_USART_NUM, "tim6 started..." );

	// adc setup (note this must be done after the tim6_counter_start() call since it uses the delay funtion)
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT1_ADC_PIN );
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT2_ADC_PIN );
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT3_ADC_PIN );
	LLPD::adc_init( ADC_NUM::ADC_1_2, ADC_CYCLES_PER_SAMPLE::CPS_64p5 );
	LLPD::adc_set_channel_order( ADC_NUM::ADC_1_2, 3, EFFECT1_ADC_CHANNEL, EFFECT2_ADC_CHANNEL, EFFECT3_ADC_CHANNEL );

	// pushbutton setup
	LLPD::gpio_digital_input_setup( EFFECT_BUTTON_PORT, EFFECT1_BUTTON_PIN, GPIO_PUPD::PULL_UP );
	LLPD::gpio_digital_input_setup( EFFECT_BUTTON_PORT, EFFECT2_BUTTON_PIN, GPIO_PUPD::PULL_UP );

	// EEPROM setup and test
	std::vector<Eeprom_CAT24C64_AddressConfig> eepromAddressConfigs;
	eepromAddressConfigs.emplace_back( EEPROM1_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM2_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM3_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM4_ADDRESS );
	Eeprom_CAT24C64_Manager_ARMor8 eeproms( EEPROM_I2C_NUM, eepromAddressConfigs );

	// LLPD::usart_log( LOGGING_USART_NUM, "Ultra_FX_SYN setup complete, entering while loop -------------------------------" );

	// setup midi handler
	MidiHandler midiHandler;
	midiHandlerPtr = &midiHandler;

	// setup preset event queue (memory comes after parameter event queue)
	uint8_t* presetEventQueueMem = reinterpret_cast<uint8_t*>( D3_SRAM_BASE ) + ( D3_SRAM_UNUSED_OFFSET_IN_BYTES )
									+ sizeof( EventQueue<ARMor8ParameterEvent> )
									+ ( sizeof(ARMor8ParameterEvent) * ARMOR8_PARAMETER_EVENT_QUEUE_SIZE );
	EventQueue<ARMor8PresetEvent>* presetEventQueue = new ( presetEventQueueMem ) EventQueue<ARMor8PresetEvent>( presetEventQueueMem
								+ sizeof(EventQueue<ARMor8PresetEvent>), sizeof(ARMor8PresetEvent)
								* ARMOR8_PRESET_EVENT_QUEUE_SIZE, 2 );
	ARMor8PresetEventBridge presetEventBridge( presetEventQueue );
	presetEventBridge.bindToARMor8PresetEventSystem();

	// inform CM4 core that setup is complete
	while ( ! LLPD::hsem_try_take(0) ) {}
	bool* volatile setupCompleteFlag = reinterpret_cast<bool*>( D2_AHBSRAM_BASE );
	*setupCompleteFlag = true;
	LLPD::hsem_release( 0 );

	// wait for CM4 core setup complete
	while ( true )
	{
		if ( LLPD::hsem_try_take(0) )
		{
			if ( *setupCompleteFlag == false )
			{
				LLPD::hsem_release( 0 );

				break;
			}

			LLPD::hsem_release( 0 );
		}
	}

	// flush denormals
	__set_FPSCR( __get_FPSCR() | (1 << 24) );

	// prepare event queues
	uint8_t* paramEventQueueMem = reinterpret_cast<uint8_t*>( D3_SRAM_BASE ) + ( D3_SRAM_UNUSED_OFFSET_IN_BYTES );
	EventQueue<ARMor8ParameterEvent>* paramEventQueue = reinterpret_cast<EventQueue<ARMor8ParameterEvent>*>( paramEventQueueMem );
	ARMor8ParameterEventBridge paramEventBridge( paramEventQueue );

	// prepare presetManager
	PresetManager presetManager( sizeof(ARMor8PresetHeader), 20, &eeproms );

	// prepare voice manager
	// ARMor8VoiceManager voiceManager( midiHandlerPtr, &presetManager ); // for interrupt-based audio
	ARMor8VoiceManager voiceManager( midiHandlerPtr, &presetManager, dmaAudioBuffer1 );
	voiceManager.bindToKeyEventSystem();
	voiceManager.bindToPitchEventSystem();
	voiceManager.bindToARMor8ParameterEventSystem();

	// define initialized preset values
	ARMor8VoiceState initPreset =
	{
		// operator 1
		500.0f,
		false,
		OscillatorMode::SINE,
		0.002f,
		0.0f,
		0.002f,
		0.0f,
		1.0f,
		0.002f,
		0.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.1f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 2
		500.0f,
		false,
		OscillatorMode::SINE,
		0.002f,
		0.0f,
		0.002f,
		0.0f,
		1.0f,
		0.002f,
		0.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 3
		500.0f,
		false,
		OscillatorMode::SINE,
		0.002f,
		0.0f,
		0.002f,
		0.0f,
		1.0f,
		0.002f,
		0.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 4
		500.0f,
		false,
		OscillatorMode::SINE,
		0.002f,
		0.0f,
		0.002f,
		0.0f,
		1.0f,
		0.002f,
		0.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// global
		false,
		0,
		0.0f,
		false,
		0,
		1,
		1,
		1,
		2,
		1
	};

	// upgrade and initialize presets if necessary
	ARMor8PresetUpgrader presetUpgrader( initPreset, voiceManager.getPresetHeader() );
	presetManager.upgradePresets( &presetUpgrader );

	// connect to audio buffer
	AudioBuffer<float> audioBuffer;
	audioBuffer.registerCallback( &voiceManager );
	audioBufferPtr = &audioBuffer;

	// load the first preset
	voiceManager.loadCurrentPreset();

	// enable instruction cache
	SCB_EnableICache();

	// enable data cache (will only be useful for constant values stored in flash)
	SCB_InvalidateDCache();
	SCB_EnableDCache();

	// remove for interrupt-based audio
	uint16_t* prevDmaBuffer = ( LLPD::dac_dma_using_buffer1() ) ? dmaAudioBuffer1 : dmaAudioBuffer2;

	while ( true )
	{
		paramEventBridge.processQueuedParameterEvents();

		LLPD::adc_perform_conversion_sequence( EFFECT_ADC_NUM );

		midiHandler.dispatchEvents();

		// for interrupt-based audio
		// audioBuffer.pollToFillBuffers();

		// for dma-based audio
		uint16_t* newDmaBuffer = ( LLPD::dac_dma_using_buffer1() ) ? dmaAudioBuffer1 : dmaAudioBuffer2;
		if ( prevDmaBuffer != newDmaBuffer )
		{
			voiceManager.setCurrentDmaBuffer( newDmaBuffer );
			audioBuffer.triggerCallbacksOnNextPoll( LLPD::dac_dma_using_buffer1() );
			audioBuffer.pollToFillBuffers();

			prevDmaBuffer = newDmaBuffer;
		}
	}
}

extern "C" void TIM6_DAC_IRQHandler (void)
{
	if ( ! LLPD::tim6_isr_handle_delay() && audioBufferPtr ) // if not currently in a delay function,...
	{
		const uint16_t outVal = static_cast<uint16_t>( (audioBufferPtr->getNextSample(0.0f) * 2047.0f) + 2048.0f );

		LLPD::dac_send( outVal, outVal );
	}

	LLPD::tim6_counter_clear_interrupt_flag();
}

extern "C" void USART2_IRQHandler (void) // logging usart
{
	/*
	// loopback test code for usart recieve
	uint16_t data = LLPD::usart_receive( LOGGING_USART_NUM );
	LLPD::usart_transmit( LOGGING_USART_NUM, data );
	*/
}

extern "C" void USART6_IRQHandler (void) // midi usart
{
	uint16_t data = LLPD::usart_receive( MIDI_USART_NUM );
	LLPD::usart_transmit( MIDI_USART_NUM, data );
	if ( midiHandlerPtr )
	{
		midiHandlerPtr->processByte( data );
	}
}
