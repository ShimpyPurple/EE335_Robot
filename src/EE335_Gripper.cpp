#include "EE335_Gripper.h"

#define DRIVER_CUSTOM_MOTOR_SHIELD 0
#define DRIVER_CUSTOM_SERVO_MANAGER 1

#define CLOSE_PERCENT 25
#define OPEN_PERCENT 75

Gripper::Gripper( uint8_t pin , MotorShield *motorShield ):
    pin( pin ) ,
    driverType( DRIVER_CUSTOM_MOTOR_SHIELD ) ,
    servoManager( servoManager )
{}

Gripper::Gripper( uint8_t pin , ServoManager *servoManager ):
    pin( pin ) ,
    driverType( DRIVER_CUSTOM_SERVO_MANAGER ) ,
    servoManager( servoManager )
{}

void Gripper::begin() {
    if ( driverType == DRIVER_CUSTOM_SERVO_MANAGER ) {
        pinMode( pin , OUTPUT );
    }
}

void Gripper::open() {
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:  motorShield->writeServo( pin , OPEN_PERCENT ); break;
        case DRIVER_CUSTOM_SERVO_MANAGER: servoManager->write(     pin , OPEN_PERCENT ); break;
    }
}

void Gripper::close() {
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:  motorShield->writeServo( pin , CLOSE_PERCENT ); break;
        case DRIVER_CUSTOM_SERVO_MANAGER: servoManager->write(     pin , CLOSE_PERCENT ); break;
    }
}

void Gripper::write( float percent ) {
    if ( percent > 100 ) percent = 100;
    if ( percent < 0 ) percent = 0;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:  motorShield->writeServo( pin , percent/100 * abs(OPEN_PERCENT - CLOSE_PERCENT) + min(CLOSE_PERCENT,OPEN_PERCENT) ); break;
        case DRIVER_CUSTOM_SERVO_MANAGER: servoManager->write(     pin , percent/100 * abs(OPEN_PERCENT - CLOSE_PERCENT) + min(CLOSE_PERCENT,OPEN_PERCENT) ); break;
    }
}
