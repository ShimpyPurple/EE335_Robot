#include "EE335_Motor.h"

Motor::Motor( MotorShield *motorShield , uint8_t motorNumber ):
    driverType( TYPE_CUSTOM_MOTOR_SHIELD ) ,
    motorShield( motorShield ) ,
    motorNumber( motorNumber ) ,
    pid( new PID(MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001) ) ,
    cruiseEnabled( false ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false )
{}

Motor::Motor( Adafruit_DCMotor *adafruitDCMotor ):
    driverType( TYPE_ADAFRUIT_DC_MOTOR ) ,
    adafruitDCMotor( adafruitDCMotor ) ,
    pid( new PID(MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001) ) ,
    cruiseEnabled( false ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false )
{}

Motor::Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 ):
    driverType( TYPE_H_BRIDGE ) ,
    pwmPin( pwmPin ) ,
    dirPin1( dirPin1 ) ,
    dirPin2( dirPin2 ) ,
    pid( new PID(MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001) ) ,
    cruiseEnabled( false ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false )
{}

void Motor::begin() {
    if ( driverType == TYPE_H_BRIDGE ) {
        pinMode( pwmPin  , OUTPUT );
        pinMode( dirPin1 , OUTPUT );
        pinMode( dirPin2 , OUTPUT );
    }
    
    setDirection( RELEASE );
    setPWM( 0 );
}

void Motor::setDirection( uint8_t direction ) {
    switch ( driverType ) {
        case TYPE_CUSTOM_MOTOR_SHIELD:
            motorShield->setMotorDirection( motorNumber , direction );
            break;
        case TYPE_ADAFRUIT_DC_MOTOR:
            adafruitDCMotor->run( direction == BRAKE ? RELEASE : direction );
            break;
        case TYPE_H_BRIDGE:
            switch ( direction ) {
                case FORWARD:  digitalWrite( dirPin2 , LOW  ); digitalWrite( dirPin1 , HIGH ); break;
                case BACKWARD: digitalWrite( dirPin1 , LOW  ); digitalWrite( dirPin2 , HIGH ); break;
                case BRAKE:    digitalWrite( dirPin1 , HIGH ); digitalWrite( dirPin2 , HIGH ); break;
                case RELEASE:  digitalWrite( dirPin1 , LOW  ); digitalWrite( dirPin2 , LOW  ); break;
            }
            break;
        default: return;
    }
    
    currentDirection = direction;
}

void Motor::requestDirection( uint8_t direction ) {
    if ( direction != currentDirection ) {
        directionToSet = direction;
        updateDirectionFlag = true;
    }
}

void Motor::setPWM( uint16_t val ) {
    switch ( driverType ) {
        case TYPE_CUSTOM_MOTOR_SHIELD:
            motorShield->setMotorPWM( motorNumber , val );
            break;
        case TYPE_ADAFRUIT_DC_MOTOR:
            if ( val == 0 ) {
                adafruitDCMotor->fullOff();
            } else if ( val >= 0x1000 ) {
                adafruitDCMotor->fullOn();
            } else {
                adafruitDCMotor->setSpeedFine( val );
            }
            break;
        case TYPE_H_BRIDGE:
            analogWrite( pwmPin , val/16 );
        default: return;
    }
    
    currentPWM = val;
}

void Motor::requestPWM( uint16_t val ) {
    if ( val != currentPWM ) {
        pwmToSet = val;
        updatePWMFlag = true;
    }
}

void Motor::setPercent( float percent ) {
    if ( percent > 100 ) percent = 100;
    if ( percent <   0 ) percent =   0;
    setPWM( percent/100 * 0x1000 );
}

void Motor::requestPercent( float percent ) {
    if ( percent > 100 ) percent = 100;
    if ( percent <   0 ) percent =   0;
    requestPWM( percent/100 * 0x1000 );
}

void Motor::update() {
    if ( updateDirectionFlag ) {
        updateDirectionFlag = false;
        setDirection( directionToSet );
    }
    if ( updatePWMFlag ) {
        updatePWMFlag = false;
        setPWM( pwmToSet );
    }
}

void Motor::attachEncoder( Encoder *encoder ) {
    this->encoder = encoder;
}

void Motor::enableCruise() {
    if ( cruiseEnabled ) return;
    cruiseEnabled = true;
    cruiseID = runAfter(
        MOTOR_PID_SAMPLE_PERIOD ,
        []( void *object ) {
            Motor *motor = ( Motor* )( object );
            motor->requestPercent( motor->pid->getControlSignal(motor->encoder->getSpeed()) );
        } ,
        this ,
        MOTOR_PID_SAMPLE_PERIOD
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
