#include "EE335_Gripper.h"

Gripper::Gripper( uint8_t pin , ServoManager *servoManager ):
    pin( pin ) ,
    servoManager( servoManager )
{}

Gripper::Gripper( uint8_t pin , uint8_t timer ):
    pin( pin )
{
    servoManager = new ServoManager( timer );
}

Gripper::Gripper( uint8_t pin , BaseTimer16 *timer16 ):
    pin( pin )
{
    servoManager = new ServoManager( timer16 );
}

Gripper::Gripper( uint8_t pin , BaseTimer8Async *timer8 ):
    pin( pin )
{
    servoManager = new ServoManager( timer8 );
}

Gripper::Gripper( uint8_t pin , GenericTimer *timer ):
    pin( pin )
{
    servoManager = new ServoManager( timer );
}

void Gripper::open() {
    servoManager->write( pin , OPEN_PERCENT );
}

void Gripper::close() {
    servoManager->write( pin , CLOSE_PERCENT );
}

void Gripper::write( float percent ) {
    if ( percent > 100 ) percent = 100;
    if ( percent < 0 ) percent = 0;
    
    servoManager->write( pin , percent/100 * abs(OPEN_PERCENT - CLOSE_PERCENT) + min(CLOSE_PERCENT,OPEN_PERCENT) );
}