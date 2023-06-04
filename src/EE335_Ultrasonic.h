#ifndef EE335_Ultrasonic_h
#define EE335_Ultrasonic_h

#include "Arduino.h"
#include "CustomInterrupts.h"
#include "CustomMotorShield.h"
#include "CustomServos.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Ultrasonic is only tested for ATmega328P and ATmega2560"
#endif

#define NUM_SWEEP_STEPS 17

class Ultrasonic {
    public:
        Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , MotorShield *motorShield , float mach=343 );
        Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , ServoManager *servoManager , float mach=343 );
        void begin();
        volatile uint8_t heading;
        volatile float range;
        void setStep( uint8_t step );
        void startSweep();
        void scanInDirection( uint8_t step );
        void stopSweep();
        bool clearSonar();
    
    private:
        uint8_t trigPin;
        uint8_t echoPin;
        uint8_t servoPin;
        uint8_t driverType;
        MotorShield *motorShield;
        ServoManager *servoManager;
        float mach;
        static void trigger( void *object );
        static void echoReceived( void *object , uint8_t edgeType );
        volatile uint32_t echoStart;
        bool clearSonarFlag;
        uint8_t sweepID;
        uint8_t sweeping;
        volatile uint8_t sweepStep;
    
};

#endif
