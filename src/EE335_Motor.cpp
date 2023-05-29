#include "EE335_Motor.h"

// --------------------------- //
//            Motor            //
// --------------------------- //

Motor::Motor( MotorShield *motorShield , uint8_t motorNumber , Encoder *encoder ):
    driverType( DRIVER_CUSTOM_MOTOR_SHIELD ) ,
    motorShield( motorShield ) ,
    motorNumber( motorNumber ) ,
    encoder( encoder ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false ) ,
    cruiseEnabled( false )
{
    if ( encoder != nullptr ) {
        pid = new PID( MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001 );
    }
}

Motor::Motor( Adafruit_DCMotor *adafruitDCMotor , Encoder *encoder ):
    driverType( DRIVER_ADAFRUIT_DC_MOTOR ) ,
    adafruitDCMotor( adafruitDCMotor ) ,
    encoder( encoder ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false ) ,
    cruiseEnabled( false )
{
    if ( encoder != nullptr ) {
        pid = new PID( MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001 );
    }
}

Motor::Motor( uint8_t pwmPin , uint8_t dirPin1 , uint8_t dirPin2 , Encoder *encoder ):
    driverType( DRIVER_H_BRIDGE ) ,
    pwmPin( pwmPin ) ,
    dirPin1( dirPin1 ) ,
    dirPin2( dirPin2 ) ,
    encoder( encoder ) ,
    currentDirection( RELEASE ) ,
    directionToSet( 0 ) ,
    updateDirectionFlag( false ) ,
    currentPWM( 0 ) ,
    pwmToSet( 0 ) ,
    updatePWMFlag( false ) ,
    cruiseEnabled( false )
{
    if ( encoder != nullptr ) {
        pid = new PID( MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001 );
    }
}

void Motor::begin() {
    if ( driverType == DRIVER_H_BRIDGE ) {
        pinMode( pwmPin  , OUTPUT );
        pinMode( dirPin1 , OUTPUT );
        pinMode( dirPin2 , OUTPUT );
    }
    
    setDirection( RELEASE );
    setPWM( 0 );
}

void Motor::setDirection( uint8_t direction ) {
    if ( direction == currentDirection ) return;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:
            motorShield->setMotorDirection( motorNumber , direction );
            break;
        case DRIVER_ADAFRUIT_DC_MOTOR:
            adafruitDCMotor->run( direction == BRAKE ? RELEASE : direction );
            break;
        case DRIVER_H_BRIDGE:
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
    if ( val == currentPWM ) return;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:
            motorShield->setMotorPWM( motorNumber , val );
            break;
        case DRIVER_ADAFRUIT_DC_MOTOR:
            if ( val == 0 ) {
                adafruitDCMotor->fullOff();
            } else if ( val >= 0x1000 ) {
                adafruitDCMotor->fullOn();
            } else {
                adafruitDCMotor->setSpeedFine( val );
            }
            break;
        case DRIVER_H_BRIDGE:
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
    pid = new PID( MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001 );
}

void Motor::enableCruise() {
    if ( cruiseEnabled ) return;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:
        case DRIVER_H_BRIDGE:
            cruiseID = runAfter(
                MOTOR_PID_SAMPLE_PERIOD ,
                []( void *object ) {
                    Motor *motor = ( Motor* )( object );
                    motor->setPercent( motor->pid->getControlSignal(motor->encoder->getSpeed()) );
                } ,
                this ,
                MOTOR_PID_SAMPLE_PERIOD
            );
            break;
        case DRIVER_ADAFRUIT_DC_MOTOR:
            cruiseID = runAfter(
                MOTOR_PID_SAMPLE_PERIOD ,
                []( void *object ) {
                    Motor *motor = ( Motor* )( object );
                    motor->requestPercent( motor->pid->getControlSignal(motor->encoder->getSpeed()) );
                } ,
                this ,
                MOTOR_PID_SAMPLE_PERIOD
            );
            break;
        default: return;
    }
    
    cruiseEnabled = true;
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

// -------------------------------- //
//            Motor Pair            //
// -------------------------------- //

MotorPair::MotorPair( Motor *m1 , Motor *m2 , Encoder *encoder ):
    m1( m1 ) ,
    m2( m2 ) ,
    driverType( m1->driverType == m2->driverType ? m1->driverType : DRIVER_UNDETERMINED ) ,
    encoder( encoder ) ,
    cruiseEnabled( false )
{
    if ( encoder != nullptr ) {
        pid = new PID( MOTOR_KP , MOTOR_KI , MOTOR_KD , MOTOR_PID_SAMPLE_PERIOD*0.001 );
    }
}

void MotorPair::begin() { m1->begin(); m2->begin(); }
void MotorPair::setDirection( uint8_t direction ) { m1->setDirection(direction); m2->setDirection(direction); }
void MotorPair::requestDirection( uint8_t direction ) { m1->requestDirection(direction); m2->requestDirection(direction); }
void MotorPair::setPWM( uint16_t val ) { m1->setPWM(val); m2->setPWM(val); }
void MotorPair::requestPWM( uint16_t val ) { m1->requestPWM(val); m2->requestPWM(val); }
void MotorPair::setPercent( float percent ) { m1->setPercent(percent); m2->setPercent(percent); }
void MotorPair::requestPercent( float percent ) { m1->requestPercent(percent); m2->requestPercent(percent); }
void MotorPair::update() { m1->update(); m2->update(); }

void MotorPair::enableCruise() {
    if ( cruiseEnabled ) return;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:
        case DRIVER_H_BRIDGE:
            cruiseID = runAfter(
                MOTOR_PID_SAMPLE_PERIOD ,
                []( void *object ) {
                    MotorPair *motorPair = ( MotorPair* )( object );
                    motorPair->setPercent( motorPair->pid->getControlSignal(motorPair->encoder->getSpeed()) );
                } ,
                this ,
                MOTOR_PID_SAMPLE_PERIOD
            );
            break;
        case DRIVER_ADAFRUIT_DC_MOTOR:
            cruiseID = runAfter(
                MOTOR_PID_SAMPLE_PERIOD ,
                []( void *object ) {
                    MotorPair *motorPair = ( MotorPair* )( object );
                    motorPair->requestPercent( motorPair->pid->getControlSignal(motorPair->encoder->getSpeed()) );
                } ,
                this ,
                MOTOR_PID_SAMPLE_PERIOD
            );
            break;
        default: return;
    }
    
    cruiseEnabled = true;
}

void MotorPair::setCruise( float speed ) {
    pid->setSetPoint( speed );
}

void MotorPair::stopCruise() {
    if ( cruiseEnabled ) {
        cruiseEnabled = false;
        runAfterCancel( cruiseID );
    }
}
