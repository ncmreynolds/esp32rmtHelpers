# Infrared helpers
This library consists of helper classes to send signals over IR, one for transmit, one for receive, per architecture.

These helpers are for when you want to make your own custom encoding/decoding and do so just by adding symbols to a buffer then calling a send function or parsing a buffer of timings yourself. Using these helper functions you can use the same code (with some minor limitations) across different platforms.

It is a solution in search of the authors own very specific problem.

## AVR

These helpers use interrupts and PWM/bit-banging.

## ESP32

These helpers use the ESP32 RMT peripheral and do all the configuration/initialisation of it for you. In principle sending IR signals is the main role of RMT but most examples are either for known encodings or uses of it to drive other custom peripherals with tight timing requirement using hardware. 
