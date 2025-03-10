# ESP32 RMT helpers
This library consists of two helper classes for using the ESP32 RMT peripheral to send signals over IR, one for transmit, one for receive.

In principle this is the main role of RMT but most examples are either for known encodings or uses of it to drive other peripherals. These helpers are for when you want to make your own custom encoding/decoding and do so just by adding symbols to a buffer then calling a send function.
