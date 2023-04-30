#ifndef EE335_Gripper_h
#define EE335_Gripper_h

#include "Arduino.h"
#include "CustomTimers.h"
#include "CustomServos.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Gripper is only tested for ATmega328P and ATmega2560"
#endif

#define CLOSE_PERCENT 25
#define OPEN_PERCENT 75

class Gripper {
    public:
        Gripper( uint8_t pin , ServoManager *servoManager );
        Gripper( uint8_t pin , uint8_t timer );
        Gripper( uint8_t pin , BaseTimer16 *timer16 );
        Gripper( uint8_t pin , BaseTimer8Async *timer8 );
        Gripper( uint8_t pin , GenericTimer *timer );
        
        void begin();
        void open();
        void close();
        void write( float percent );
    
    private:
        uint8_t pin;
        ServoManager *servoManager;
    
};

#endif
