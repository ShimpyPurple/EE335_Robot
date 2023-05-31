#ifndef EE335_PID_h
#define EE335_PID_h

#include "Arduino.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Gripper is only tested for ATmega328P and ATmega2560"
#endif

class PID {
    public:
        PID( float kp , float ki , float kd , float samplePeriod );
        void setSetPoint( float setPoint );
        float getControlSignal( float processOutput );
    
    private:
        const float kp;
        const float ki;
        const float kd;
        const float samplePeriod;
        float setPoint;
        float integral;
        float prevError;
    
};

#endif
