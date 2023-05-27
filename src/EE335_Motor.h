#ifndef EE335_Motor_h
#define EE335_Motor_h

#include "Arduino.h"
#include "Adafruit_MotorShield.h"
#include "EE335_Encoder.h"
#include "Constants/Constants.h"
#include "PID/EE335_PID.h"
#include "CustomInterrupts.h"
#include "CustomMotorShield.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Motor is only tested for ATmega328P and ATmega2560"
#endif

#define TYPE_CUSTOM_MOTOR_SHIELD 0
#define TYPE_ADAFRUIT_DC_MOTOR 1
#define TYPE_H_BRIDGE 3

class Motor {
    public:
        Motor( MotorShield *motorShield , uint8_t motorNumber );
        Motor( Adafruit_DCMotor *adafruitDCMotor );
        Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 );
        void begin();
        void setDirection( uint8_t direction );
        void requestDirection( uint8_t direction );
        void setPWM( uint16_t val );
        void requestPWM( uint16_t val );
        void setPercent( float percent );
        void requestPercent( float percent );
        void update();
        void attachEncoder( Encoder *encoder );
        void enableCruise();
        void setCruise( float speed );
        void stopCruise();
        
    private:
        const uint8_t driverType;
        const MotorShield *motorShield;
        const uint8_t motorNumber;
        const Adafruit_DCMotor *adafruitDCMotor;
        const uint8_t pwmPin;
        const uint8_t dirPin1;
        const uint8_t dirPin2;
        uint8_t currentDirection;
        uint8_t directionToSet;
        bool updateDirectionFlag;
        uint8_t currentPWM;
        uint8_t pwmToSet;
        bool updatePWMFlag;
        Encoder *encoder;
        const PID *pid;
        bool cruiseEnabled;
        uint8_t cruiseID;
    
};

#endif
