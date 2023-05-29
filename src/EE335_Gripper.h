#ifndef EE335_Gripper_h
#define EE335_Gripper_h

#include "Arduino.h"
#include "CustomMotorShield.h"
#include "CustomServos.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Gripper is only tested for ATmega328P and ATmega2560"
#endif

#define DRIVER_CUSTOM_MOTOR_SHIELD 0
#define DRIVER_CUSTOM_SERVO_MANAGER 1

#define CLOSE_PERCENT 25
#define OPEN_PERCENT 75

class Gripper {
    public:
        Gripper( uint8_t pin , MotorShield *motorShield );
        Gripper( uint8_t pin , ServoManager *servoManager );
        void begin();
        void open();
        void close();
        void write( float percent );
    
    private:
        uint8_t pin;
        uint8_t driverType;
        MotorShield *motorShield;
        ServoManager *servoManager;
    
};

#endif
