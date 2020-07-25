#include "../lib/STM32f302x8-HAL/llpd/include/LLPD.hpp"

#include "PolyBLEPOsc.hpp"
#include "OnePoleFilter.hpp"

const int SYS_CLOCK_FREQUENCY = 32000000;
const int EEPROM_SIZE = 8192; // EEPROM is CAT24C64
const int SRAM_SIZE = 65536; // SRAM is 23A256/23K256

volatile float someFloat = 0.0f;
volatile bool isOn = false;
volatile bool keepBlinking = true; // a test variable, will be set to false if peripheral operations are invalid
volatile int ledIncr = 0; // should flash led every time this value is equal to ledMax
volatile int ledMax = 20000;
volatile uint16_t dacVal = 0;

PolyBLEPOsc* osc;
float oscFreq = 500.0f;
OnePoleFilter* filt;

void writeDataToSRAM (uint16_t address, uint8_t data)
{
	// pull cs low
	LLPD::gpio_output_set( GPIO_PORT::B, GPIO_PIN::PIN_12, false );

	// send write instruction
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, 0b00000010 );

	// send first half of address
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, (address >> 8) );

	// send second half of address
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, (address & 0b0000000011111111) );

	// send data
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, data );

	// pull cs high
	LLPD::gpio_output_set( GPIO_PORT::B, GPIO_PIN::PIN_12, true );
}

uint8_t readDataFromSRAM (uint16_t address)
{
	// pull cs low
	LLPD::gpio_output_set( GPIO_PORT::B, GPIO_PIN::PIN_12, false );

	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, 0b00000011 );

	// send first half of address
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, (address >> 8) );

	// send second half of address
	LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, (address & 0b0000000011111111) );

	// recieve data
	uint8_t data = LLPD::spi_master_send_and_recieve( SPI_NUM::SPI_2, 0b00000000 );

	// pull cs high
	LLPD::gpio_output_set( GPIO_PORT::B, GPIO_PIN::PIN_12, true );

	// return data from spi
	return data;
}

void writeDataToEEPROM (uint16_t address, uint8_t data)
{
	// store address in two bytes
	uint8_t addrH = (address >> 8);
	uint8_t addrL = (address & 0b0000000011111111);

	// set address to A0 = low, A1 = low, A2 = low
	LLPD::i2c_master_set_slave_address( I2C_NUM::I2C_2, I2C_ADDR_MODE::BITS_7, 0b01010000 );

	LLPD::i2c_master_write( I2C_NUM::I2C_2, true, 3, addrH, addrL, data );
}

uint8_t readDataFromEEPROM (uint16_t address)
{
	// store address in two bytes
	uint8_t addrH = (address >> 8);
	uint8_t addrL = (address & 0b0000000011111111);

	// set address to A0 = low, A1 = low, A2 = low
	LLPD::i2c_master_set_slave_address( I2C_NUM::I2C_2, I2C_ADDR_MODE::BITS_7, 0b01010000 );

	// send address bytes
	LLPD::i2c_master_write( I2C_NUM::I2C_2, false, 2, addrH, addrL );

	// variable to store data
	uint8_t data = 45;
	LLPD::i2c_master_read( I2C_NUM::I2C_2, true, 1, &data );

	return data;
}

int main(void)
{
	PolyBLEPOsc polyBlepOsc;
	osc = &polyBlepOsc;
	osc->setFrequency( 500.0f );
	osc->setOscillatorMode( OscillatorMode::SAWTOOTH );
	OnePoleFilter onePFilt;
	filt = &onePFilt;
	filt->setCoefficients( oscFreq );

	// clock setup
	LLPD::rcc_clock_setup( RCC_CLOCK_SOURCE::EXTERNAL, false );
	LLPD::rcc_pll_setup( RCC_CLOCK_SOURCE::EXTERNAL, false, RCC_PLL_MULTIPLY::BY_2 );

	// enable all gpio clocks directly after clock setup
	LLPD::gpio_enable_clock( GPIO_PORT::A );
	LLPD::gpio_enable_clock( GPIO_PORT::B );
	LLPD::gpio_enable_clock( GPIO_PORT::C );

	// i2c setup (8MHz source 100KHz clock 0x00201D2B, 32MHz source 100KHz clock 0x00B07CB4)
	LLPD::i2c_master_setup( I2C_NUM::I2C_2, 0x00B07CB4 );

	// spi init
	LLPD::spi_master_init( SPI_NUM::SPI_2, SPI_BAUD_RATE::SYSCLK_DIV_BY_2, SPI_CLK_POL::LOW_IDLE, SPI_CLK_PHASE::FIRST,
				SPI_DUPLEX::FULL, SPI_FRAME_FORMAT::MSB_FIRST, SPI_DATA_SIZE::BITS_8 );

	// now set up our cs pin, which we will use b12
	LLPD::gpio_output_setup( GPIO_PORT::B, GPIO_PIN::PIN_12, GPIO_PUPD::NONE, GPIO_OUTPUT_TYPE::PUSH_PULL,
				GPIO_OUTPUT_SPEED::HIGH, false );

	// set cs high
	LLPD::gpio_output_set( GPIO_PORT::B, GPIO_PIN::PIN_12, true );

	// LED pin
	LLPD::gpio_output_setup( GPIO_PORT::A, GPIO_PIN::PIN_0, GPIO_PUPD::NONE, GPIO_OUTPUT_TYPE::PUSH_PULL,
					GPIO_OUTPUT_SPEED::HIGH );

	// pushbutton setup example
	LLPD::gpio_digital_input_setup( GPIO_PORT::A, GPIO_PIN::PIN_1, GPIO_PUPD::PULL_DOWN );

	// audio timer setup (for 40 kHz sampling rate at 32 MHz system clock)
	LLPD::tim6_counter_setup( 1, 800, 40000 );
	LLPD::tim6_counter_enable_interrupts();

	// DAC setup
	LLPD::dac_init( true );

	// audio timer start
	LLPD::tim6_counter_start();

	// ADC setup (note, this must be done after the tim6_counter_start() call since it uses the delay function)
	LLPD::gpio_analog_setup( GPIO_PORT::A, GPIO_PIN::PIN_1 ); // channel 2
	LLPD::gpio_analog_setup( GPIO_PORT::A, GPIO_PIN::PIN_2 ); // channel 3
	LLPD::gpio_analog_setup( GPIO_PORT::A, GPIO_PIN::PIN_3 ); // channel 4
	LLPD::gpio_analog_setup( GPIO_PORT::A, GPIO_PIN::PIN_6 ); // channel 10
	LLPD::adc_init( ADC_CYCLES_PER_SAMPLE::CPS_601p5 );
	LLPD::adc_set_channel_order( 4, ADC_CHANNEL::CHAN_2, ADC_CHANNEL::CHAN_3, ADC_CHANNEL::CHAN_4, ADC_CHANNEL::CHAN_10 );

	// USART setup
	LLPD::usart_init( USART_NUM::USART_3, USART_WORD_LENGTH::BITS_8, USART_PARITY::EVEN, USART_CONF::TX_AND_RX,
				USART_STOP_BITS::BITS_1, SYS_CLOCK_FREQUENCY, 9600 );
	// quick usart transmission test
	for ( uint16_t val = 0; val < 256; val++ )
	{
		LLPD::usart_transmit( USART_NUM::USART_3, val );
	}

	/*
	// test all addresses in SRAM
	for ( int address = 0; address < SRAM_SIZE; address++)
	{
		uint8_t dataToWrite = address % 256;

		writeDataToSRAM( address, dataToWrite );

		uint8_t dataRead = readDataFromSRAM(address);

		if ( dataRead == dataToWrite && keepBlinking == true )
		{
			keepBlinking = true;
		}
		else
		{
			keepBlinking = false;
			break;
		}
	}

	// test all addresses in EEPROM
	for ( int address = 0; address < EEPROM_SIZE; address++ )
	{
		uint8_t dataToWrite = address % 256;

		writeDataToEEPROM( address, dataToWrite );

		if ( readDataFromEEPROM(address) == dataToWrite && keepBlinking == true )
		{
			keepBlinking = true;
		}
		else
		{
			keepBlinking = false;
			break;
		}
	}
	*/

	while (1)
	{
		// do conversion on 4 channels (note: even though we set channel order, adc_gets can be called in any order)
		LLPD::adc_perform_conversion_sequence();
		// uint16_t chan2Val = LLPD::adc_get_channel_value( ADC_CHANNEL::CHAN_2 );
		// uint16_t chan3Val = LLPD::adc_get_channel_value( ADC_CHANNEL::CHAN_3 );
		// uint16_t chan4Val = LLPD::adc_get_channel_value( ADC_CHANNEL::CHAN_4 );
		uint16_t chan10Val = LLPD::adc_get_channel_value( ADC_CHANNEL::CHAN_10 );

		ledMax = chan10Val;

		/*
		// test pushbutton
		if ( LLPD::gpio_digital_input_get(GPIO_PORT::A, GPIO_PIN::PIN_1) )
		{
			LLPD::gpio_output_set( GPIO_PORT::A, GPIO_PIN::PIN_0, false );
		}
		*/
	}
}

extern "C" void TIM6_DAC_IRQHandler (void)
{

	if ( !LLPD::tim6_isr_handle_delay() ) // if not currently in a delay function,...
	{
		oscFreq += 0.1f;
		if ( oscFreq > 2000.0f ) oscFreq = 50.0f;
		filt->setCoefficients( oscFreq );
		float sampleVal = filt->processSample( osc->nextSample() );
		dacVal = static_cast<uint16_t>( sampleVal * 1000.0f );

		LLPD::dac_send( dacVal );

		if ( keepBlinking && ledIncr > ledMax )
		{
			if ( isOn )
			{
				LLPD::gpio_output_set( GPIO_PORT::A, GPIO_PIN::PIN_0, false );
				isOn = false;
			}
			else
			{
				LLPD::gpio_output_set( GPIO_PORT::A, GPIO_PIN::PIN_0, true );
				isOn = true;
			}

			ledIncr = 0;
		}
		else
		{
			ledIncr++;
		}
	}

	LLPD::tim6_counter_clear_interrupt_flag();
}

extern "C" void USART3_IRQHandler (void)
{
	// loopback test code for usart recieve
	uint16_t data = LLPD::usart_receive( USART_NUM::USART_3 );
	LLPD::usart_transmit( USART_NUM::USART_3, data );
}
