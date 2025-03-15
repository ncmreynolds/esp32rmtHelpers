#ifndef avrInfraredHelpers_cpp
#define avrInfraredHelpers_cpp
#if defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_NANO
#include <avrInfraredHelpers.h>

//Transmit helpers
avrInfraredTransmitHelper::avrInfraredTransmitHelper()	//Constructor function
{
}
avrInfraredTransmitHelper::~avrInfraredTransmitHelper()	//Destructor function
{
}
void avrInfraredTransmitHelper::setCarrierFrequency(uint16_t frequency)				//Must be done before begin(), default is 56000
{
}
void avrInfraredTransmitHelper::setDutyCycle(uint8_t duty, uint8_t transmitterIndex)	//Must be done before begin(), default is 50 and very unlikely to change
{
}
bool avrInfraredTransmitHelper::begin(uint8_t numberOfTransmitters)	//Set up transmitters
{
	return false;
}
bool avrInfraredTransmitHelper::configureTxPin(uint8_t index, int8_t pin)				//Configure a pin for TX on the specified channel
{
	return false;
}
bool avrInfraredTransmitHelper::addSymbol(uint8_t index, uint16_t duration0, uint8_t level0, uint16_t duration1, uint8_t level1)		//Add a symbol to the buffer for the specified transmitter channel
{
	return false;
}
bool avrInfraredTransmitHelper::transmitSymbols(uint8_t transmitterIndex, bool wait)			//Transmit a buffer from the specified transmitter channel
{
	return false;
}
bool avrInfraredTransmitHelper::transmitterBusy(uint8_t index)									//Used to check if busy before starting another transmission
{
	return false;
}
avrInfraredTransmitHelper transmitHelper;	//Create an instance of the class, as only one is practically usable at a time despite not being a singleton

//Receive helpers
avrInfraredReceiveHelper::avrInfraredReceiveHelper()	//Constructor function
{
	setMaximumNumberOfSymbols(getMinimumNumberOfSymbols());
}
avrInfraredReceiveHelper::~avrInfraredReceiveHelper()	//Destructor function
{
}
bool avrInfraredReceiveHelper::begin(uint8_t numberOfReceivers)
{
	bool initialisation_success_ = true;
	number_of_receivers_ = numberOfReceivers;														//Record the number of receivers
	received_symbols_ = new uint16_t*[number_of_receivers_];										//Create array of symbol buffer(s)
	waiting_for_symbols_ = new bool[number_of_receivers_];
	number_of_received_symbols_ = new uint8_t[number_of_receivers_];								//Create array of symbol buffer length(s)
	last_edge_ = new uint32_t[number_of_receivers_];												//Edge timers
	current_edge_ = new uint16_t[number_of_receivers_];												//Edge timer index
	for(uint8_t index = 0; index < number_of_receivers_; index++)
	{
		received_symbols_[index] = new uint16_t[getMaximumNumberOfSymbols()*2];						//Create symbol buffers
		for(uint8_t symbolIndex = 0; symbolIndex < getMaximumNumberOfSymbols()*2; symbolIndex++)
		{
			received_symbols_[index][symbolIndex] = 0;												//Blank out the buffer, just in case
		}
		number_of_received_symbols_[index] = 0;														//Set received buffer length to zero
		last_edge_[index] = 0;																		//Clear last edge time
		current_edge_[index] = 0;																	//Reset symbol index
		waiting_for_symbols_[index] = true;															//Be ready to accept symbols
	}
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("avrInfraredReceiveHelper: configuring "));
		debug_uart_->print(numberOfReceivers);
		debug_uart_->print(F(" receiver(s) for up to "));
		debug_uart_->print(getMaximumNumberOfSymbols());
		debug_uart_->println(F(" symbols"));
	}
	return initialisation_success_;
}
inline void avrInfraredReceiveHelperIsr(uint8_t index)
{
	if(receiveHelper.waiting_for_symbols_[index] == true)
	{
		if(receiveHelper.current_edge_[index] > 0 && micros() - receiveHelper.last_edge_[index] > 5000)	//Pulse must be 'first' or within 5ms of the last so reset to start, this filters out stray readings
		{
			receiveHelper.current_edge_[index] = 0;
			Serial.println("Timing buffer auto-reset");
		}
		if(micros() - receiveHelper.last_edge_[index] < 5000)	//Continue reading edges
		{
			receiveHelper.received_symbols_[index][receiveHelper.current_edge_[index]++] = (micros() - receiveHelper.last_edge_[index])>>3;
		}
		receiveHelper.last_edge_[index] = micros();
		if(receiveHelper.current_edge_[index] == receiveHelper.getMaximumNumberOfSymbols()*2) //Stop accepting timings because we're out of array!
		{
			receiveHelper.waiting_for_symbols_[index] = false;
			receiveHelper.current_edge_[index]--;
			Serial.println("Timing buffer overflow");
		}
	}
}
void avrInfraredReceiveHelperIsr0()
{
	avrInfraredReceiveHelperIsr(0);
}
void avrInfraredReceiveHelperIsr1()
{
	avrInfraredReceiveHelperIsr(1);
}
bool avrInfraredReceiveHelper::configureRxPin(uint8_t index, int8_t pin, bool inverted)				//Configure a pin for RX on the current available channel
{
	if(index < 2)
	{
		pinMode(pin, INPUT);
		delay(500);
		if(digitalRead(pin) == false)
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("avrInfraredReceiveHelper: warning pin "));
				debug_uart_->print(pin);
				debug_uart_->print(F(" not connected to receiver\r\n"));
			}
			return true;
		}
		switch (index)
		{
			case 0:
				attachInterrupt(digitalPinToInterrupt(pin), avrInfraredReceiveHelperIsr0, CHANGE);
			break;
			case 1:
				attachInterrupt(digitalPinToInterrupt(pin), avrInfraredReceiveHelperIsr1, CHANGE);
			break;
			default:
				return false;
			break;
		}
		waiting_for_symbols_[index] = true;
		if(true)
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("avrInfraredReceiveHelper: configured pin "));
				debug_uart_->print(pin);
				debug_uart_->print(F(" for RX\r\n"));
			}
			return true;
		}
		else
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("avrInfraredReceiveHelper: failed to configure pin "));
				debug_uart_->print(pin);
				debug_uart_->print(F(" for RX\r\n"));
			}
		}
	}
	return false;
}
uint8_t avrInfraredReceiveHelper::receivedSymbolLevel0(uint8_t index, uint16_t symbolIndex)			//Getter for the symbol data
{
	return 1;	//Not recording this so assume level0 is high
	//return (symbolIndex & 0x0001) == 0;	//Assume even durations are low
}
uint8_t avrInfraredReceiveHelper::receivedSymbolLevel1(uint8_t index, uint16_t symbolIndex)			//Getter for the symbol data
{
	return 0;	//Not recording this so assume level1 is low
	//return (symbolIndex & 0x0001) != 0;	//Assume odd durations are high
}
uint16_t avrInfraredReceiveHelper::receivedSymbolDuration0(uint8_t index, uint16_t symbolIndex)		//Getter for the symbol data
{
	return uint16_t(received_symbols_[index][symbolIndex<<1])<<3;	//Even entries are duration0
}
uint16_t avrInfraredReceiveHelper::receivedSymbolDuration1(uint8_t index, uint16_t symbolIndex)		//Getter for the symbol data
{
	return uint16_t(received_symbols_[index][1+(symbolIndex<<1)])<<3;	//Odd entries are duration1
}
uint8_t avrInfraredReceiveHelper::getNumberOfReceivedSymbols(uint8_t index)
{
	if(index < number_of_receivers_)
	{
		if(waiting_for_symbols_[index] == true)
		{
			if(current_edge_[index] > 0 && micros() - last_edge_[index] > 5000)			//It's been 5ms since an edge so we've received some IR symbols
			{
				if(debug_uart_ != nullptr)
				{
					debug_uart_->print(F("avrInfraredReceiveHelper: pausing reception on channel "));
					debug_uart_->println(index);
				}
				waiting_for_symbols_[index] = false;
				if((current_edge_[index] & 0x0001) == 0)
				{
					receiveHelper.received_symbols_[index][receiveHelper.current_edge_[index]++] = 0;	//The last level is always low and duration zero
				}
				number_of_received_symbols_[index] = current_edge_[index]>>1;							//Update symbol count from symbol buffer index
				return number_of_received_symbols_[index];										//Inform the caller there's a usefully filled symbol buffer
			}
		}
		else	//There are symbols in the buffer to process
		{
			if(number_of_received_symbols_[index] == 0)
			{
				if(current_edge_[index] == 1)
				{
					current_edge_[index] = 0;
					waiting_for_symbols_[index] = true;
				}
				else
				{
					number_of_received_symbols_[index] = current_edge_[index]>>1;	//Update symbol count from symbol buffer index
					return number_of_received_symbols_[index];						//Inform the caller there's a usefully filled symbol buffer
				}
			}
		}
	}
	return 0;
}
void avrInfraredReceiveHelper::resume(uint8_t index)
{
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("avrInfraredReceiveHelper: resuming reception on channel "));
		debug_uart_->println(index);
	}
	number_of_received_symbols_[index] = 0;
	last_edge_[index] = 0;
	current_edge_[index] = 0;
	waiting_for_symbols_[index] = true;
}
avrInfraredReceiveHelper receiveHelper;	//Create an instance of the class, as only one is practically usable at a time despite not being a singleton

#endif
#endif
