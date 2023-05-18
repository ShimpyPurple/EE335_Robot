#include "EE335_Ultrasonic.h"

int stepPositions[NUM_SWEEP_STEPS] = {
    0 , 5 , 11 , 16 , 21 , 27 , 32 , 38 , 43 , 49 , 56 , 62 , 69 , 75 , 81 , 88 , 94
};

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , ServoManager *servoManager , float mach=343 ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoPin( servoPin ) ,
    servoManager( servoManager ) ,
    mach( mach ) ,
    sweeping( NOT_SWEEPING )
{}

void Ultrasonic::begin() {
    pinMode( trigPin , OUTPUT );
    pinMode( echoPin , INPUT );
    pinMode( servoPin , OUTPUT );
    
    setStep( NUM_SWEEP_STEPS/2 );
    attachInterruptCustom( echoPin , CHANGE , echoReceived , this );
}

static void Ultrasonic::trigger( void *object ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    
    digitalWrite( ultrasonic->trigPin , HIGH );
    delayMicroseconds( 10 );
    digitalWrite( ultrasonic->trigPin , LOW );
}

void Ultrasonic::setStep( uint8_t step ) {
    // servoManager->write( servoPin , SWEEP_START + (float)step/NUM_SWEEP_STEPS * (SWEEP_END - SWEEP_START) );
    servoManager->write( servoPin , stepPositions[step] );
    sweepStep = step;
}

void Ultrasonic::startSweep() {
    sweeping = SWEEPING_UP;
    setStep( 0 );
    runAfter( 150 , trigger , this , true );
}

static void Ultrasonic::echoReceived( void *object , uint8_t edgeType ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    uint32_t us = micros();
    
    switch ( edgeType ) {
        case RISING:
            ultrasonic->echoStart = us;
            break;
        case FALLING:
            ultrasonic->distance[ultrasonic->sweepStep] = ( us - ultrasonic->echoStart ) * ultrasonic->mach / 2 / 1e6;
            switch ( ultrasonic->sweeping ) {
                case SWEEPING_DOWN:
                    if ( ultrasonic->sweepStep == 0 ) {
                        ultrasonic->setStep( ultrasonic->sweepStep + 1 );
                        ultrasonic->sweeping = SWEEPING_UP;
                    } else {
                        ultrasonic->setStep( ultrasonic->sweepStep - 1 );
                    }
                    // runAfter( 100 , trigger , object );
                    break;
                case SWEEPING_UP:
                    if ( ultrasonic->sweepStep == NUM_SWEEP_STEPS-1 ) {
                        ultrasonic->setStep( ultrasonic->sweepStep - 1 );
                        ultrasonic->sweeping = SWEEPING_DOWN;
                    } else {
                        ultrasonic->setStep( ultrasonic->sweepStep + 1 );
                    }
                    // runAfter( 100 , trigger , object );
                    break;
            }
            break;
    }
}
