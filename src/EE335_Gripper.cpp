#include "EE335_Gripper.h"

Gripper::Gripper( uint8_t pin , ServoManager servoManager ):
    pin( pin ) ,
    servoManager( servoManager )
{
    
}

Gripper::Gripper( uint8_t pin , uint8_t timer ):
    pin( pin ) ,
{
    servoManager = new ServoManager( timer );
}
