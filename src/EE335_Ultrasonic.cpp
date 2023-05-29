#include "EE335_Ultrasonic.h"

uint16_t triggerCount = 0;

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , MotorShield *motorShield , float mach=343 ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoPin( servoPin ) ,
    driverType( TYPE_CUSTOM_MOTOR_SHIELD ) ,
    motorShield( motorShield ) ,
    mach( mach ) ,
    sweeping( NOT_SWEEPING )
{}

Ultrasonic::Ultrasonic( uint8_t trigPin , uint8_t echoPin , uint8_t servoPin , ServoManager *servoManager , float mach=343 ):
    trigPin( trigPin ) ,
    echoPin( echoPin ) ,
    servoPin( servoPin ) ,
    driverType( TYPE_CUSTOM_SERVO_MANAGER ) ,
    servoManager( servoManager ) ,
    mach( mach ) ,
    sweeping( NOT_SWEEPING )
{}

void Ultrasonic::begin() {
    pinMode( trigPin , OUTPUT );
    pinMode( echoPin , INPUT );
    if ( driverType == TYPE_CUSTOM_SERVO_MANAGER ) {
        pinMode( servoPin , OUTPUT );
    }
    
    attachInterruptCustom( echoPin , CHANGE , echoReceived , this );
}

static void Ultrasonic::trigger( void *object ) {
    Ultrasonic *ultrasonic = ( Ultrasonic* )( object );
    
    digitalWrite( ultrasonic->trigPin , HIGH );
    delayMicroseconds( 10 );
    digitalWrite( ultrasonic->trigPin , LOW );
    
    if ( ++triggerCount >= 1000 ) {
        ultrasonic->stopSweep();
    }
}

void Ultrasonic::setStep( uint8_t step ) {
    switch ( driverType ) {
        case TYPE_CUSTOM_MOTOR_SHIELD:
            motorShield->writeServo32( servoPin , (float)32/(NUM_SWEEP_STEPS-1) * step );
            break;
        case TYPE_CUSTOM_SERVO_MANAGER:
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
        sweepID = runAfter( 1000 , trigger , this , 100 );
    }
}

void Ultrasonic::stopSweep() {
    if( sweeping != NOT_SWEEPING ) {
        sweeping = NOT_SWEEPING;
        runAfterCancel( sweepID );
    }
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
            break;
    }
}
