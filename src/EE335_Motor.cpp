#include "EE335_Motor.h"

Motor::Motor( Adafruit_DCMotor *adafruitDCMotor ):
    adafruitDCMotor( adafruitDCMotor )
{}

Motor::Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 ):
    adafruitDCMotor( nullptr ) ,
    pwmPin( pwmPin ) ,
    dirPin1( dirPin1 ) ,
    dirPin2( dirPin2 )
{}

void Motor::begin() {
    if ( adafruitDCMotor == nullptr ) {
        pinMode( pwmPin  , OUTPUT );
        pinMode( dirPin1 , OUTPUT );
        pinMode( dirPin2 , OUTPUT );
    }
}

void Motor::setPwm( uint8_t val ) {
    if ( adafruitDCMotor == nullptr ) {
        analogWrite( pwmPin , val );
    } else {
        adafruitDCMotor->setSpeed( val );
    }
}

void Motor::setDirection( uint8_t direction ) {
    if ( adafruitDCMotor == nullptr ) {
        switch ( direction ) {
            case FORWARD:
                digitalWrite( dirPin2 , LOW );
                digitalWrite( dirPin1 , HIGH );
                break;
            case BACKWARD:
                digitalWrite( dirPin1 , LOW );
                digitalWrite( dirPin2 , HIGH );
                break;
            case RELEASE:
                digitalWrite( dirPin1 , LOW );
                digitalWrite( dirPin2 , LOW );
                break;
        }
    } else {
        adafruitDCMotor->run( direction );
    }
}
