#include "EE335_Ultrasonic.h"

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , ServoManager *servoManager , float mach=34300 ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoManager( servoManager ) ,
    mach( mach )
{}

void Ultrasonic::begin() {
    pinMode( trigPin , OUTPUT );
    pinMode( echoPin , INPUT );
    
    factor = mach / ( 2 * timer->getTickRate() );
}

float Ultrasonic::getDistance() {
    
}

static void Ultrasonic::echoReceived( void *object , uint8_t edgeType ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
}
