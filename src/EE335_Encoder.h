#ifndef EE335_Encoder_h
#define EE335_Encoder_h

#include "Arduino.h"
#include "Constants/Constants.h"
#include "CustomTimers.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Encoder is only tested for ATmega328P and ATmega2560"
#endif

#define MAX_PERIOD 2000000

class Encoder {
    public:
        Encoder( uint8_t pin , float wheelCircumference , uint8_t holesPerRevolution );
        float getSpeed();
        void begin();
    
    private:
        uint8_t pin;
        float factor;
        uint32_t lastEdge;
        uint32_t period;
        static void onRisingEdge( void *object );
        
};

#endif
