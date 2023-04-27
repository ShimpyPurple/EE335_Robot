#include "EE335_Motor.h"

Motor::Motor( uint8_t pwmPin , uint8_t directionPin1 , uint8_t directionPin2 ):
    pwmPin( pwmPin ) ,
    directionPin1( directionPin1 ) ,
    directionPin2( directionPin2 )
{
	pinMode( pwmPin        , OUTPUT );
	pinMode( directionPin1 , OUTPUT );
	pinMode( directionPin2 , OUTPUT );
    
    setDirection( FORWARD );
}

void Motor::setPwm( uint8_t dutyCycle ) {
	analogWrite( pwmPin , dutyCycle );
}

void Motor::setDirection( uint8_t direction ) {
	digitalWrite( directionPin1 , direction );
	digitalWrite( directionPin2 , !direction );
}
