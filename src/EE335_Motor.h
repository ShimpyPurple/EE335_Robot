#ifndef EE335_Motor_h
#define EE335_Motor_h

#include "Arduino.h"
#include "Constants/Constants.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Motor is only tested for ATmega328P and ATmega2560"
#endif

class Motor {
    public:
        Motor( uint8_t pwmPin , uint8_t directionPin1 , uint8_t directionPin2 );
        void setPwm( uint8_t dutyCycle );
        void setDirection( uint8_t dutyCycle );
        
    private:
        uint8_t pwmPin;
        uint8_t directionPin1;
        uint8_t directionPin2;
};

#endif
