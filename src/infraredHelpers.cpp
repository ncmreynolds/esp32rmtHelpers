#ifndef infraredHelpers_cpp
#define infraredHelpers_cpp
#include <Arduino.h>						//Standard Arduino library

#include <infraredHelpers.h>

infraredHelpers::infraredHelpers()	//Constructor function
{
}

infraredHelpers::~infraredHelpers()	//Destructor function
{
}


void infraredHelpers::debug(Stream &terminalStream)
{
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("infraredHelpers: debug enabled\r\n"));
	}
}

#endif