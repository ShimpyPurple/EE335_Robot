#ifndef EE335_Lineread_h
#define EE335_Lineread_h

#include "Arduino.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Lineread is only tested for ATmega328P and ATmega2560"
#endif

class Lineread {
    public:
        Lineread( uint8_t pin , void (*onLineEdge)(uint8_t) );
        void begin();
    
    private:
        uint8_t pin;
        void ( *onLineEdge )( uint8_t );
        
};

#endif
