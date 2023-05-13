#ifndef EE335_Ultrasonic_h
#define EE335_Ultrasonic_h

#include "Arduino.h"
#include "CustomServos.h"
#include "Constants/Constants.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Ultrasonic is only tested for ATmega328P and ATmega2560"
#endif

class Ultrasonic {
    public:
        Ultrasonic( uint8_t trigPin , uint8_t echoPin , ServoManager *servoManager , float mach );
        void begin();
        float getDistance();
    
    private:
        uint8_t trigPin;
        uint8_t echoPin;
        ServoManager *servoManager;
        float mach;
        float factor;
    
};

#endif
