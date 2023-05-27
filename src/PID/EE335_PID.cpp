#include "EE335_PID.h"

PID::PID( float kp , float ki , float kd , float samplePeriod ):
    kp( kp ) ,
    ki( ki ) ,
    kd( kd ) ,
    samplePeriod( samplePeriod ) ,
    setPoint( 0 ) ,
    integral( 0 ) ,
    prevError( 0 )
{}

void PID::setSetPoint( float setPoint ) {
    this->setPoint = setPoint;
}

float PID::getControlSignal( float processOutput ) {
    float error = setPoint - processOutput;
    float derivative = ( error - prevError ) / samplePeriod;
    integral += error * samplePeriod;
    
    prevError = error;
    
    return kp*error + ki*integral + kd*derivative;
}
