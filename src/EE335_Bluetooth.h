#ifndef EE335_Bluetooth_h
#define EE335_Bluetooth_h

#include "Arduino.h"
#include "Constants/Constants.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Bluetooth is only tested for ATmega328P and ATmega2560"
#endif

class Bluetooth {
    public:
        Bluetooth( uint8_t serialPort , uint8_t rxMode );
        void getInstruction();
        struct {
            uint8_t rightJoystickRadius = 0;
            uint8_t leftJoystickRadius  = 0;
            float rightJoystickAngle = 0;
            float leftJoystickAngle  = 0;
            uint8_t rightTrigger = 0;
            uint8_t leftTrigger  = 0;
            bool rightBumper = false;
            bool leftBumper  = false;
            bool dPadUp    = false;
            bool dPadDown  = false;
            bool dPadRight = false;
            bool dPadLeft  = false;
            bool buttonA = false;
            bool buttonB = false;
            bool buttonX = false;
            bool buttonY = false;
            bool select = false; 
            bool start  = false;
            bool mode   = false;
        } ControllerState;
    
    private:
        HardwareSerial *btSerial;
        bool noSerial;
        uint8_t rxMode;
        void getControllerInstruction();
        void getPlainTextInstruction();
        void waitForSerial();
};

#endif
