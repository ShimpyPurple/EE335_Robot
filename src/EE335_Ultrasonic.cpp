#include "EE335_Ultrasonic.h"

#define NOT_SWEEPING 0
#define SWEEPING_DOWN 1
#define SWEEPING_UP 2

#define DRIVER_CUSTOM_MOTOR_SHIELD 0
#define DRIVER_CUSTOM_SERVO_MANAGER 1

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , MotorShield *motorShield , float mach ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoPin( servoPin ) ,
    driverType( DRIVER_CUSTOM_MOTOR_SHIELD ) ,
    motorShield( motorShield ) ,
    mach( mach ) ,
    sweeping( NOT_SWEEPING )
{}

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , ServoManager *servoManager , float mach ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoPin( servoPin ) ,
    driverType( DRIVER_CUSTOM_SERVO_MANAGER ) ,
    servoManager( servoManager ) ,
    mach( mach ) ,
    sweeping( NOT_SWEEPING )
{}

void Ultrasonic::begin() {
    pinMode( trigPin , OUTPUT );
    pinMode( echoPin , INPUT );
    if ( driverType == DRIVER_CUSTOM_SERVO_MANAGER ) {
        pinMode( servoPin , OUTPUT );
    }
    
    attachInterruptCustom( echoPin , CHANGE , echoReceived , this );
}

void Ultrasonic::setStep( uint8_t step ) {
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:
            motorShield->writeServo32( servoPin , (float)32/(NUM_SWEEP_STEPS-1) * step );
            break;
        case DRIVER_CUSTOM_SERVO_MANAGER:
            servoManager->write( servoPin , (float)step/(NUM_SWEEP_STEPS-1) * 100 );
            break;
        default: return;
    }
    sweepStep = step;
}

void Ultrasonic::startSweep() {
    if( sweeping == NOT_SWEEPING ) {
        sweeping = SWEEPING_UP;
        setStep( 0 );
        sweepID = runAfter( 1000 , trigger , this , 250 );
    }
}

void Ultrasonic::stopSweep() {
    if( sweeping != NOT_SWEEPING ) {
        sweeping = NOT_SWEEPING;
        runAfterCancel( sweepID );
    }
}

void Ultrasonic::trigger( void *object ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    
    digitalWrite( ultrasonic->trigPin , HIGH );
    delayMicroseconds( 10 );
    digitalWrite( ultrasonic->trigPin , LOW );
    
    runAfter(
        160 ,
        []( void *object ) {
            Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
            switch ( ultrasonic->sweeping ) {
                case SWEEPING_DOWN:
                    if ( ultrasonic->sweepStep == 0 ) {
                        ultrasonic->setStep( ultrasonic->sweepStep + 1 );
                        ultrasonic->sweeping = SWEEPING_UP;
                    } else {
                        ultrasonic->setStep( ultrasonic->sweepStep - 1 );
                    }
                    break;
                case SWEEPING_UP:
                    if ( ultrasonic->sweepStep == NUM_SWEEP_STEPS-1 ) {
                        ultrasonic->setStep( ultrasonic->sweepStep - 1 );
                        ultrasonic->sweeping = SWEEPING_DOWN;
                    } else {
                        ultrasonic->setStep( ultrasonic->sweepStep + 1 );
                    }
                    break;
            }
        } ,
        object
    );
    
}

void Ultrasonic::echoReceived( void *object , uint8_t edgeType ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    uint32_t us = micros();
    
    switch ( edgeType ) {
        case RISING:
            ultrasonic->echoStart = us;
            break;
        case FALLING:
            ultrasonic->heading = ultrasonic->sweepStep;
            ultrasonic->range = ( us - ultrasonic->echoStart ) * ultrasonic->mach / 2 / 1e6;
            break;
    }
}
