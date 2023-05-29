#ifndef EE335_Bluetooth_h
#define EE335_Bluetooth_h

#include "Arduino.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Bluetooth is only tested for ATmega328P and ATmega2560"
#endif

#define SERIAL_0 0
#define SERIAL_1 1
#define SERIAL_2 2
#define SERIAL_3 3

#define ALL_HEADINGS 0xFF

class Bluetooth {
    public:
        Bluetooth( uint8_t serialPort , void (**onRx)(Bluetooth*)=nullptr , uint16_t sendCooldown=10 );
        void begin();
        bool getInstruction();
        bool instructionReceived;
        struct {
            uint8_t rightJoystickRadius = 0;
            uint8_t leftJoystickRadius  = 0;
            float rightJoystickAngle = 0;
            float leftJoystickAngle  = 0;
            uint8_t rightTrigger = 0;
            uint8_t leftTrigger  = 0;
            bool dPadUp    = false;
            bool dPadDown  = false;
            bool dPadRight = false;
            bool dPadLeft  = false;
            bool buttonA = false;
            bool buttonB = false;
            bool buttonX = false;
            bool buttonY = false;
            bool rightBumper = false;
            bool leftBumper  = false;
            bool select = false;
            bool start  = false;
        } controllerState;
        struct {
            uint8_t speed = 0;
            uint8_t cruiseSpeed = 0;
            bool cruiseControl = false;
            bool lineFollowing = false;
            void setSpeed( float speed , float minSpeed , float maxSpeed ) {
                if ( speed < minSpeed ) speed = minSpeed;
                if ( speed > maxSpeed ) speed = maxSpeed;
                this->speed = ( speed - minSpeed ) / ( maxSpeed - minSpeed );
            }
            void setCruiseSpeed( float cruiseSpeed , float minSpeed , float maxSpeed ) {
                if ( cruiseSpeed < minSpeed ) cruiseSpeed = minSpeed;
                if ( cruiseSpeed > maxSpeed ) cruiseSpeed = maxSpeed;
                this->cruiseSpeed = ( cruiseSpeed - minSpeed ) / ( maxSpeed - minSpeed );
            }
        } speedometerState;
        struct {
            uint8_t heading;
            uint8_t range;
            void setRange( float range , float minRange , float maxRange ) {
                if ( range < minRange ) range = minRange;
                if ( range > maxRange ) range = maxRange;
                this->range = ( range - minRange ) / ( maxRange - minRange );
            }
        } sonarState;
        void sendState();
    
    private:
        HardwareSerial *btSerial;
        uint16_t sendCooldown;
        void waitForSerial();
        struct {
            uint8_t speed = 0;
            uint8_t cruiseSpeed = 0;
            bool cruiseControl = false;
            bool lineFollowing = false;
        } prevSpeedometerState;
        struct {
            uint8_t heading;
            uint8_t range;
        } prevSonarState;
        uint32_t lastSendTime;
    
};

#endif
