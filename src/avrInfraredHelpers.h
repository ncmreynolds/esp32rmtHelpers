#ifndef avrInfraredHelpers_h
#define avrInfraredHelpers_h

class avrInfraredTransmitHelper : public infraredHelpers, public infraredTransmitHelper {
};

class avrInfraredReceiveHelper : public infraredHelpers, public infraredTransmitHelper {
};

#endif
