#include "EE335_PID.h"

PID::PID( float kp , float ki ):
    kp( kp ) , ki( ki ) , setPoint( 0 )
{}

void PID::setSetPoint( float setPoint ) {
    this->setPoint = setPoint;
}

float PID::getControlSignal( float processOutput ) {
    uint32_t ms = millis();
    
    float error = setPoint - processOutput;
    integral += error * ( ms - prevSample ) / 1000.0;
    
    prevSample = ms;
    
    return kp*error + ki*integral;
}