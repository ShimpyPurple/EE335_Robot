#ifndef EE335_Encoder_h
#define EE335_Encoder_h

#include "Arduino.h"
#include "CustomTimers.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Encoder is only tested for ATmega328P and ATmega2560"
#endif

class Encoder {
    public:
        Encoder( uint8_t pin , float wheelCircumference , uint8_t holesPerRevolution );
        float getDistance();
        float getSpeed();
        void begin();
    
    private:
        uint8_t pin;
        float distancePerHole;
        float distance;
        uint32_t lastEdge;
        uint32_t period;
        static void onRisingEdge( void *object );
        
};

#endif
