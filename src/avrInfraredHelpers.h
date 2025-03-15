#ifndef avrInfraredHelpers_h
#define avrInfraredHelpers_h
#if defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_NANO
#include <Arduino.h>						//Standard Arduino library
#include <infraredHelpers.h>				//Infrared helpers

class avrInfraredTransmitHelper : public infraredHelpers, public infraredTransmitHelper {

	public:
		avrInfraredTransmitHelper();														//Constructor function
		~avrInfraredTransmitHelper();														//Destructor function
		void setCarrierFrequency(uint16_t frequency);										//Must be done before begin(), default is 56000
		void setDutyCycle(uint8_t duty, uint8_t transmitterIndex = 0);						//Must be done before begin(), default is 50 and very unlikely to change
		bool begin(uint8_t numberOfTransmitters = 1);										//Initialise one or more transmitters
		bool configureTxPin(uint8_t index, int8_t pin);										//Configure a pin for TX on the current available channel
		bool addSymbol(uint8_t index, uint16_t duration0, uint8_t level0,					//Add a symbol to the buffer for the specified transmitter channel
			uint16_t duration1, uint8_t level1);
		bool transmitSymbols(uint8_t transmitterIndex,										//Transmit a buffer from the specified transmitter channel
				bool wait = false);
		bool transmitterBusy(uint8_t index);												//Used to check if busy before starting another transmission

	protected:
	
	private:
};
extern avrInfraredTransmitHelper transmitHelper;	//Create an instance of the class, as only one is practically usable at a time despite not being a singleton

class avrInfraredReceiveHelper : public infraredHelpers, public infraredReceiveHelper {

	public:
		avrInfraredReceiveHelper();															//Constructor function
		~avrInfraredReceiveHelper();														//Destructor function
		bool begin(uint8_t numberOfReceivers = 1);											//Initialise one or more receivers
		bool configureRxPin(uint8_t index, int8_t pin, bool inverted = true);				//Configure a pin for RX on the current available channel
		uint8_t receivedSymbolLevel0(uint8_t index, uint16_t symbolIndex);					//Getters for the symbol data
		uint8_t receivedSymbolLevel1(uint8_t index, uint16_t symbolIndex);
		uint16_t receivedSymbolDuration0(uint8_t index, uint16_t symbolIndex);
		uint16_t receivedSymbolDuration1(uint8_t index, uint16_t symbolIndex);
		uint8_t getNumberOfReceivedSymbols(uint8_t index);									//Number of symbols received by a receiver (overridden to do packet detection)
		void resume(uint8_t index);															//Resume reception on a specific channel

		uint16_t** received_symbols_ = nullptr;												//Symbol buffers
		bool* waiting_for_symbols_ = nullptr;												//Flag if currently receiving 'edges'
		uint32_t* last_edge_ = nullptr;														//Edge timers
		uint16_t* current_edge_ = nullptr;													//Edge timers

	protected:																				//Similar to the ESP32 RTM symbol data structure
	private:
};
inline void avrInfraredReceiveHelperIsr(uint8_t index) __attribute__((always_inline));			//Make sure the ISR is always inline
extern avrInfraredReceiveHelper receiveHelper;	//Create an instance of the class, as only one is practically usable at a time despite not being a singleton
#endif
#endif
