#include "EE335_Ultrasonic.h"

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , ServoManager *servoManager , float mach=343 ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoManager( servoManager ) ,
    mach( mach )
{}

void Ultrasonic::begin() {
    pinMode( trigPin , OUTPUT );
    pinMode( echoPin , INPUT );
    
    servoManager->writeMicros( trigPin , 10 ); //Will round up to 50us, but that's fine.
    attachInterruptCustom( echoPin , CHANGE , echoReceived , this );
}

static void Ultrasonic::echoReceived( void *object , uint8_t edgeType ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    
    switch ( edgeType ) {
        case RISING:
            ultrasonic->echoStart = micros();
            break;
        case FALLING:
            ultrasonic->distance = ( micros() - ultrasonic->echoStart ) * ultrasonic->mach / 2 / 1e6;
            break;
    }
}
