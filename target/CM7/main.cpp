#include "../../lib/STM32h745zi-HAL/llpd/include/LLPD.hpp"

#include "EEPROM_CAT24C64.hpp"
#include "SRAM_23K256.hpp"
#include "SDCard.hpp"
#include "EventQueue.hpp"
#include "IEventListener.hpp"
#include "MidiHandler.hpp"

#define SYS_CLOCK_FREQUENCY = 480000000;

// global variables
volatile bool adcSetupComplete = false; // should be set to true after adc has been initialized
MidiHandler* volatile midiHandlerPtr = nullptr;

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

// TODO delete this after testing
#include "IKeyEventListener.hpp"

class KeyEventTester : public IKeyEventListener
{
	public:
		KeyEventTester() {}
		~KeyEventTester() override {}

		void onKeyEvent (const KeyEvent& keyEvent) override
		{
			if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
			{
				LLPD::usart_log_int( LOGGING_USART_NUM, "Key pressed: ", keyEvent.note() );
			}
			else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
			{
				LLPD::usart_log_int( LOGGING_USART_NUM, "Key released: ", keyEvent.note() );
			}
		}
};



// these pins are unconnected on Ultra_FX_SYN Rev 2 development board, so we disable them as per the ST recommendations
void disableUnusedPins()
{
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
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

	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_2, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_4, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_5, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
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

	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_0, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::C, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
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
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_10, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::E, GPIO_PIN::PIN_11, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );

	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_6, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::LOW );
	LLPD::gpio_output_setup( GPIO_PORT::F, GPIO_PIN::PIN_8, GPIO_PUPD::PULL_DOWN, GPIO_OUTPUT_TYPE::PUSH_PULL,
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
}

int main(void)
{
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

	// USART setup
	LLPD::usart_init( LOGGING_USART_NUM, USART_WORD_LENGTH::BITS_8, USART_PARITY::NONE, USART_CONF::TX_AND_RX,
				USART_STOP_BITS::BITS_1, 120000000, 9600 );
	LLPD::usart_log( LOGGING_USART_NUM, "Ultra_FX_SYN starting up ----------------------------" );
	LLPD::usart_init( MIDI_USART_NUM, USART_WORD_LENGTH::BITS_8, USART_PARITY::EVEN, USART_CONF::TX_AND_RX,
					USART_STOP_BITS::BITS_1, 120000000, 31250 );

	// audio timer setup (for 40 kHz sampling rate at 480 MHz timer clock)
	LLPD::tim6_counter_setup( 0, 6000, 40000 );
	LLPD::tim6_counter_enable_interrupts();
	LLPD::usart_log( LOGGING_USART_NUM, "tim6 initialized..." );

	// DAC setup
	LLPD::dac_init( true );
	LLPD::usart_log( LOGGING_USART_NUM, "dac initialized..." );

	// Op Amp setup
	LLPD::gpio_analog_setup( OP_AMP1_INV_OUT_PORT, OP_AMP1_INVERT_PIN );
	LLPD::gpio_analog_setup( OP_AMP1_INV_OUT_PORT, OP_AMP1_OUTPUT_PIN );
	LLPD::gpio_analog_setup( OP_AMP1_NONINVR_PORT, OP_AMP1_NON_INVERT_PIN );
	LLPD::opamp_init( OPAMP_NUM::OPAMP_1 );
	LLPD::gpio_analog_setup( OP_AMP2_PORT, OP_AMP2_INVERT_PIN );
	LLPD::gpio_analog_setup( OP_AMP2_PORT, OP_AMP2_OUTPUT_PIN );
	LLPD::gpio_analog_setup( OP_AMP2_PORT, OP_AMP2_NON_INVERT_PIN );
	LLPD::opamp_init( OPAMP_NUM::OPAMP_2 );
	LLPD::usart_log( LOGGING_USART_NUM, "op amp initialized..." );

	// spi initialization
	LLPD::spi_master_init( SRAM_SPI_NUM, SPI_BAUD_RATE::SYSCLK_DIV_BY_256, SPI_CLK_POL::LOW_IDLE, SPI_CLK_PHASE::FIRST,
				SPI_DUPLEX::FULL, SPI_FRAME_FORMAT::MSB_FIRST, SPI_DATA_SIZE::BITS_8 );
	LLPD::spi_master_init( SD_CARD_SPI_NUM, SPI_BAUD_RATE::SYSCLK_DIV_BY_256, SPI_CLK_POL::LOW_IDLE, SPI_CLK_PHASE::FIRST,
				SPI_DUPLEX::FULL, SPI_FRAME_FORMAT::MSB_FIRST, SPI_DATA_SIZE::BITS_8 );
	LLPD::spi_master_init( OLED_SPI_NUM, SPI_BAUD_RATE::SYSCLK_DIV_BY_256, SPI_CLK_POL::LOW_IDLE, SPI_CLK_PHASE::FIRST,
				SPI_DUPLEX::FULL, SPI_FRAME_FORMAT::MSB_FIRST, SPI_DATA_SIZE::BITS_8 );
	LLPD::usart_log( LOGGING_USART_NUM, "spi initialized..." );

	// i2c initialization
	LLPD::i2c_master_setup( EEPROM_I2C_NUM, 0x308075AE );
	LLPD::usart_log( LOGGING_USART_NUM, "i2c initialized..." );

	// audio timer start
	LLPD::tim6_counter_start();
	LLPD::usart_log( LOGGING_USART_NUM, "tim6 started..." );

	// adc setup (note this must be done after the tim6_counter_start() call since it uses the delay funtion)
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT1_ADC_PIN );
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT2_ADC_PIN );
	LLPD::gpio_analog_setup( EFFECT_ADC_PORT, EFFECT3_ADC_PIN );
	LLPD::gpio_analog_setup( AUDIO_IN_PORT, AUDIO1_IN_PIN );
	LLPD::gpio_analog_setup( AUDIO_IN_PORT, AUDIO2_IN_PIN );
	LLPD::adc_init( ADC_NUM::ADC_1_2, ADC_CYCLES_PER_SAMPLE::CPS_64p5 );
	LLPD::adc_init( ADC_NUM::ADC_3, ADC_CYCLES_PER_SAMPLE::CPS_32p5 );
	LLPD::adc_set_channel_order( ADC_NUM::ADC_1_2, 3, EFFECT1_ADC_CHANNEL, EFFECT2_ADC_CHANNEL, EFFECT3_ADC_CHANNEL );
	LLPD::adc_set_channel_order( ADC_NUM::ADC_3, 2, AUDIO1_IN_ADC_CHANNEL, AUDIO2_IN_ADC_CHANNEL );
	adcSetupComplete = true;

	// pushbutton setup
	LLPD::gpio_digital_input_setup( EFFECT_BUTTON_PORT, EFFECT1_BUTTON_PIN, GPIO_PUPD::PULL_UP );
	LLPD::gpio_digital_input_setup( EFFECT_BUTTON_PORT, EFFECT2_BUTTON_PIN, GPIO_PUPD::PULL_UP );

	// SRAM setup and test
	std::vector<Sram_23K256_GPIO_Config> spiGpioConfigs;
	spiGpioConfigs.emplace_back( SRAM_CS_PORT, SRAM1_CS_PIN );
	spiGpioConfigs.emplace_back( SRAM_CS_PORT, SRAM2_CS_PIN );
	spiGpioConfigs.emplace_back( SRAM_CS_PORT, SRAM3_CS_PIN );
	spiGpioConfigs.emplace_back( SRAM_CS_PORT, SRAM4_CS_PIN );
	Sram_23K256_Manager srams( SRAM_SPI_NUM, spiGpioConfigs );
	SharedData<uint8_t> sramValsToWrite = SharedData<uint8_t>::MakeSharedData( 3 );
	sramValsToWrite[0] = 25; sramValsToWrite[1] = 16; sramValsToWrite[2] = 8;
	srams.writeToMedia( sramValsToWrite, 45 );
	srams.writeToMedia( sramValsToWrite, 45 + Sram_23K256::SRAM_SIZE );
	srams.writeToMedia( sramValsToWrite, 45 + Sram_23K256::SRAM_SIZE * 2 );
	srams.writeToMedia( sramValsToWrite, 45 + Sram_23K256::SRAM_SIZE * 3 );
	SharedData<uint8_t> sram1Verification = srams.readFromMedia( 3, 45 );
	SharedData<uint8_t> sram2Verification = srams.readFromMedia( 3, 45 + Sram_23K256::SRAM_SIZE );
	SharedData<uint8_t> sram3Verification = srams.readFromMedia( 3, 45 + Sram_23K256::SRAM_SIZE * 2 );
	SharedData<uint8_t> sram4Verification = srams.readFromMedia( 3, 45 + Sram_23K256::SRAM_SIZE * 3 );
	if ( sram1Verification[0] == 25 && sram1Verification[1] == 16 && sram1Verification[2] == 8 &&
			sram2Verification[0] == 25 && sram2Verification[1] == 16 && sram2Verification[2] == 8 &&
			sram3Verification[0] == 25 && sram3Verification[1] == 16 && sram3Verification[2] == 8 &&
			sram4Verification[0] == 25 && sram4Verification[1] == 16 && sram4Verification[2] == 8 )
	{
		LLPD::usart_log( LOGGING_USART_NUM, "srams verified..." );
	}
	else
	{
		LLPD::usart_log( LOGGING_USART_NUM, "WARNING!!! srams failed verification..." );
	}

	// EEPROM setup and test
	std::vector<Eeprom_CAT24C64_AddressConfig> eepromAddressConfigs;
	eepromAddressConfigs.emplace_back( EEPROM1_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM2_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM3_ADDRESS );
	eepromAddressConfigs.emplace_back( EEPROM4_ADDRESS );
	Eeprom_CAT24C64_Manager eeproms( EEPROM_I2C_NUM, eepromAddressConfigs );
	// TODO comment the verification lines out if you're using the eeprom for persistent memory
	SharedData<uint8_t> eepromValsToWrite = SharedData<uint8_t>::MakeSharedData( 3 );
	eepromValsToWrite[0] = 64; eepromValsToWrite[1] = 23; eepromValsToWrite[2] = 17;
	eeproms.writeToMedia( eepromValsToWrite, 45 );
	eeproms.writeToMedia( eepromValsToWrite, 45 + Eeprom_CAT24C64::EEPROM_SIZE );
	SharedData<uint8_t> eeprom1Verification = eeproms.readFromMedia( 3, 45 );
	SharedData<uint8_t> eeprom2Verification = eeproms.readFromMedia( 3, 45 + Eeprom_CAT24C64::EEPROM_SIZE );
	if ( eeprom1Verification[0] == 64 && eeprom1Verification[1] == 23 && eeprom1Verification[2] == 17 &&
			eeprom2Verification[0] == 64 && eeprom2Verification[1] == 23 && eeprom2Verification[2] == 17 )
	{
		LLPD::usart_log( LOGGING_USART_NUM, "eeproms verified..." );
	}
	else
	{
		LLPD::usart_log( LOGGING_USART_NUM, "WARNING!!! eeproms failed verification..." );
	}

	LLPD::usart_log( LOGGING_USART_NUM, "Ultra_FX_SYN setup complete, entering while loop -------------------------------" );

	// setup midi handler
	MidiHandler midiHandler;
	midiHandlerPtr = &midiHandler;

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

	// TODO for testing, delete later
	uint8_t* eventQueueMem = reinterpret_cast<uint8_t*>( D3_SRAM_BASE ) + ( D3_SRAM_UNUSED_OFFSET_IN_BYTES );
	EventQueue<IEvent>* eventQueue = reinterpret_cast<EventQueue<IEvent>*>( eventQueueMem );
	KeyEventTester keyEventTester;
	keyEventTester.bindToKeyEventSystem();

	while ( true )
	{
		LLPD::adc_perform_conversion_sequence( EFFECT_ADC_NUM );

		// TODO testing stuff, remove later
		IEvent iEvent( 0 );
		bool readCorrectly = eventQueue->readEvent( iEvent );
		if ( readCorrectly )
		{
			// LLPD::usart_log_int( LOGGING_USART_NUM, "Got event! Event Number: ", iEvent.getChannel() );
		}

		midiHandler.dispatchEvents();
	}
}

extern "C" void TIM6_DAC_IRQHandler (void)
{
	if ( ! LLPD::tim6_isr_handle_delay() ) // if not currently in a delay function,...
	{
		if ( adcSetupComplete )
		{
			LLPD::adc_perform_conversion_sequence( AUDIO_IN_ADC_NUM );
			uint16_t audioIn1 = LLPD::adc_get_channel_value( AUDIO_IN_ADC_NUM, AUDIO1_IN_ADC_CHANNEL );
			uint16_t audioIn2 = LLPD::adc_get_channel_value( AUDIO_IN_ADC_NUM, AUDIO2_IN_ADC_CHANNEL );
			LLPD::dac_send( audioIn1, audioIn2 );
		}
	}

	LLPD::tim6_counter_clear_interrupt_flag();
}

extern "C" void USART2_IRQHandler (void) // logging usart
{
	// loopback test code for usart recieve
	uint16_t data = LLPD::usart_receive( LOGGING_USART_NUM );
	LLPD::usart_transmit( LOGGING_USART_NUM, data );
}

extern "C" void USART6_IRQHandler (void) // midi usart
{
	uint16_t data = LLPD::usart_receive( MIDI_USART_NUM );
	if ( midiHandlerPtr )
	{
		midiHandlerPtr->processByte( data );
	}
}
