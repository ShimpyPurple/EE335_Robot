#ifndef EE335_Lineread_h
#define EE335_Lineread_h

#include "Arduino.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Lineread is only tested for ATmega328P and ATmega2560"
#endif

#define LINEREAD_RIGHT 0
#define LINEREAD_LEFT  1

class Lineread {
    public:
        Lineread( uint8_t rightPin , uint8_t leftPin , void (*onLineEdge)(uint8_t , uint8_t) );
        void begin();
        void startFollowing();
        void stopFollowing();
        bool isFollowing();
    
    private:
        uint8_t rightPin;
        uint8_t leftPin;
        void ( *onLineEdge )( uint8_t , uint8_t );
        bool following;
        
};

#endif
