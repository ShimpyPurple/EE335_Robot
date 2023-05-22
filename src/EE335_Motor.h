#ifndef EE335_Motor_h
#define EE335_Motor_h

#include "Arduino.h"
#include "Adafruit_MotorShield.h"
#include "EE335_Encoder.h"
#include "Constants/Constants.h"
#include "PID/EE335_PID.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Motor is only tested for ATmega328P and ATmega2560"
#endif

class Motor {
    public:
        Motor( Adafruit_DCMotor *adafruitDCMotor );
        Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 );
        void begin();
        void setDirection( uint8_t direction );
        void requestDirection( uint8_t val );
        void setPWM( uint8_t val );
        void requestPWM( uint8_t val );
        void update();
        void attachEncoder( Encoder *encoder );
        void enableCruise();
        void setCruise( float speed );
        void stopCruise();
        
    private:
        Adafruit_DCMotor *adafruitDCMotor;
        uint8_t pwmPin;
        uint8_t dirPin1;
        uint8_t dirPin2;
        uint8_t currentDirection;
        uint8_t directionToSet;
        bool updateDirectionFlag;
        uint8_t currentPWM;
        uint8_t pwmToSet;
        bool updatePWMFlag;
        Encoder *encoder;
        PID *pid;
        bool cruiseEnabled;
        uint8_t cruiseID;
    
};

#endif
