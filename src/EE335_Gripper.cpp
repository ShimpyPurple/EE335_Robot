#include "EE335_Gripper.h"

#define DRIVER_CUSTOM_MOTOR_SHIELD 0
#define DRIVER_CUSTOM_SERVO_MANAGER 1

#define CLOSE_PERCENT 19
#define OPEN_PERCENT 80

Gripper::Gripper( uint8_t pin , MotorShield *motorShield ):
    pin( pin ) ,
    driverType( DRIVER_CUSTOM_MOTOR_SHIELD ) ,
    motorShield( motorShield ) ,
    position( OPEN_PERCENT ) ,
    slowWriteID( 0 )
{}

Gripper::Gripper( uint8_t pin , ServoManager *servoManager ):
    pin( pin ) ,
    driverType( DRIVER_CUSTOM_SERVO_MANAGER ) ,
    servoManager( servoManager ) ,
    position( OPEN_PERCENT ) ,
    slowWriteID( 0 )
{}

void Gripper::begin() {
    if ( driverType == DRIVER_CUSTOM_SERVO_MANAGER ) {
        pinMode( pin , OUTPUT );
    }
    open();
}

void Gripper::write( float percent ) {
    if ( percent > 100 ) percent = 100;
    if ( percent < 0 ) percent = 0;
    
    switch ( driverType ) {
        case DRIVER_CUSTOM_MOTOR_SHIELD:  motorShield->writeServo( pin , percent/100 * abs(OPEN_PERCENT - CLOSE_PERCENT) + min(CLOSE_PERCENT,OPEN_PERCENT) ); break;
        case DRIVER_CUSTOM_SERVO_MANAGER: servoManager->write(     pin , percent/100 * abs(OPEN_PERCENT - CLOSE_PERCENT) + min(CLOSE_PERCENT,OPEN_PERCENT) ); break;
        default: return;
    }
    position = percent;
}

void Gripper::open() {
    write( OPEN_PERCENT );
}

void Gripper::close() {
    write( CLOSE_PERCENT );
}

void Gripper::slowWrite( float percent ) {
    stopSlowWrite();
    targetPosition = percent;
    slowWriteID = runAfter(
        100 ,
        []( void *object ) {
            Gripper *gripper = ( Gripper* )( object );
            if ( abs(gripper->position - gripper->targetPosition) < 5 ) {
                gripper->write( gripper->position );
                gripper->stopSlowWrite();
            } else {
                gripper->write(
                    gripper->position < gripper->targetPosition ?
                    gripper->position+5 :
                    gripper->position-5
                );
            }
        } ,
        this ,
        100
    );
}

void Gripper::slowOpen() {
    slowWrite( OPEN_PERCENT );
}

void Gripper::slowClose() {
    slowWrite( CLOSE_PERCENT );
}

void Gripper::stopSlowWrite() {
    if ( slowWriteID != 0 ) {
        runAfterCancel( slowWriteID );
        slowWriteID = 0;
    }
}
