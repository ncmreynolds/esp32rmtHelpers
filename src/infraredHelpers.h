#ifndef infraredHelpers_h
#define infraredHelpers_h
#include <Arduino.h>						//Standard Arduino library


class infraredHelpers
{
	public:
		infraredHelpers();												//Constructor function
		~infraredHelpers();												//Destructor function

		void debug(Stream &);											//Enable debugging on a stream, eg. Serial, which must already be started
		Stream *debug_uart_ = nullptr;									//The stream used for debugging
	
	protected:
	
	private:
};

#if defined ESP32
	#include <esp32rmtHelpers.h>			//Use the RMT peripheral for ESP32
#elif defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_NANO
	#include <avrInfraredHelpers.h>			//Use bitbang and interrupts
#endif

#endif