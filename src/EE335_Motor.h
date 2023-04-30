#ifndef EE335_Motor_h
#define EE335_Motor_h

#include "Arduino.h"
#include "Adafruit_MotorShield.h"
#include "Constants/Constants.h"


#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Motor is only tested for ATmega328P and ATmega2560"
#endif

class Motor {
    public:
        Motor( Adafruit_DCMotor *adafruitDCMotor );
        Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 );
        void begin();
        void setPwm( uint8_t dutyCycle );
        void setDirection( uint8_t dutyCycle );
        
    private:
        Adafruit_DCMotor *adafruitDCMotor;
        uint8_t pwmPin;
        uint8_t dirPin1;
        uint8_t dirPin2;
};

#endif
