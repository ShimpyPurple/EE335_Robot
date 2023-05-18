#ifndef EE335_Ultrasonic_h
#define EE335_Ultrasonic_h

#include "Arduino.h"
#include "CustomInterrupts.h"
#include "CustomServos.h"
#include "Constants/Constants.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Ultrasonic is only tested for ATmega328P and ATmega2560"
#endif

#define NUM_SWEEP_STEPS 17
#define SWEEP_START 0
#define SWEEP_END 95

#define NOT_SWEEPING 0
#define SWEEPING_DOWN 1
#define SWEEPING_UP 2

class Ultrasonic {
    public:
        Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , ServoManager *servoManager , float mach=343 );
        void begin();
        volatile float distance[NUM_SWEEP_STEPS] = { 0 };
        void setStep( uint8_t step );
        void startSweep();
    
    private:
        uint8_t trigPin;
        uint8_t echoPin;
        uint8_t servoPin;
        ServoManager *servoManager;
        float mach;
        uint8_t sweeping;
        uint8_t sweepStep;
        static void trigger( void *object );
        static void echoReceived( void *object , uint8_t edgeType );
        volatile uint32_t echoStart;
    
};

#endif
