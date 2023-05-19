#include "EE335_Motor.h"

Motor::Motor( Adafruit_DCMotor *adafruitDCMotor ):
    adafruitDCMotor( adafruitDCMotor ) ,
    pid( new PID(1,3) ) ,
    cruiseEnabled( false ) ,
    pwmToSet( 0 ) ,
    updatePwmFlag( false )
{}

Motor::Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 ):
    adafruitDCMotor( nullptr ) ,
    pwmPin( pwmPin ) ,
    dirPin1( dirPin1 ) ,
    dirPin2( dirPin2 ) ,
    pid( new PID(1,3) ) ,
    cruiseEnabled( false ) ,
    pwmToSet( 0 ) ,
    updatePwmFlag( false )
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

void Motor::attachEncoder( Encoder *encoder ) {
    this->encoder = encoder;
}

void Motor::enableCruise() {
    cruiseEnabled = true;
    cruiseID = runAfter(
        50 ,
        []( void *object ) {
            Motor *motor = ( Motor* )( object );
            motor->pwmToSet = motor->pid->getControlSignal( motor->encoder->getSpeed() );
            motor->updatePwmFlag = true;
        } ,
        this ,
        true
    );
}

void Motor::setCruise( float speed ) {
    pid->setSetPoint( speed );
}

void Motor::stopCruise() {
    if ( cruiseEnabled ) {
        cruiseEnabled = false;
        runAfterCancel( cruiseID );
    }
}

void Motor::updatePwm() {
    if ( updatePwmFlag ) {
        updatePwmFlag = false;
        setPwm( pwmToSet );
    }
}