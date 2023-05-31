#ifndef EE335_Motor_h
#define EE335_Motor_h

#include "Arduino.h"
#include "Adafruit_MotorShield.h"
#include "EE335_Encoder.h"
#include "PID/EE335_PID.h"
#include "CustomInterrupts.h"
#include "CustomMotorShield.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Motor is only tested for ATmega328P and ATmega2560"
#endif

class Motor {
    friend class MotorPair;
    
    public:
        Motor( MotorShield *motorShield , uint8_t motorNumber , Encoder *encoder=nullptr );
        Motor( Adafruit_DCMotor *adafruitDCMotor , Encoder *encoder=nullptr );
        Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 , Encoder *encoder=nullptr );
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
        uint8_t driverType;
        MotorShield *motorShield;
        uint8_t motorNumber;
        Adafruit_DCMotor *adafruitDCMotor;
        uint8_t pwmPin;
        uint8_t dirPin1;
        uint8_t dirPin2;
        Encoder *encoder;
        uint8_t currentDirection;
        uint8_t directionToSet;
        bool updateDirectionFlag;
        uint16_t currentPWM;
        uint16_t pwmToSet;
        bool updatePWMFlag;
        PID *pid;
        bool cruiseEnabled;
        uint8_t cruiseID;
    
};

class MotorPair {
    
    public:
        MotorPair( Motor *m1 , Motor *m2 , Encoder *encoder );
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
        Motor *m1;
        Motor *m2;
        uint8_t driverType;
        Encoder *encoder;
        PID *pid;
        bool cruiseEnabled;
        uint8_t cruiseID;
    
};

#endif
