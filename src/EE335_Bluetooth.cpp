#include "EE335_Bluetooth.h"

void idle() __attribute__( (weak) );

Bluetooth::Bluetooth( uint8_t serialPort , void (**onRx)(Bluetooth*) , uint16_t sendCooldown ):
    sendCooldown( sendCooldown ) ,
    lastSendTime( 0 ) ,
    instructionReceived( false )
{
    switch ( serialPort ) {
        case SERIAL_0: btSerial = &Serial;  break;
#if defined( __AVR_ATmega2560__ )
        case SERIAL_1: btSerial = &Serial1; break;
        case SERIAL_2: btSerial = &Serial2; break;
        case SERIAL_3: btSerial = &Serial3; break;
#endif
        default: btSerial = &Serial;  break;
    }
    
    if ( onRx != nullptr ) {
        *onRx = []( Bluetooth *object ) {
            Bluetooth *bluetooth = ( Bluetooth* )( object );
            if ( bluetooth->getInstruction() ) {
                bluetooth->instructionReceived = true;
            }
        };
    }
}

void Bluetooth::begin() {
    btSerial->begin( 9600 );
}

bool Bluetooth::getInstruction() {
    if ( !btSerial->available() ) return false;
    
    uint8_t header = btSerial->read();
    
    if ( header & (1<<R_JOYSTICK) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        if ( inByte < R1_START ) {
            controllerState.rightJoystickRadius = 0;
            controllerState.rightJoystickAngle  = 0;
        } else if ( inByte < R2_START ) {
            controllerState.rightJoystickRadius = 1;
            controllerState.rightJoystickAngle  = (float)( inByte - R1_START ) / ( R2_START - R1_START );
        } else if ( inByte < R3_START ) {
            controllerState.rightJoystickRadius = 2;
            controllerState.rightJoystickAngle  = (float)( inByte - R2_START ) / ( R3_START - R2_START );
        } else if ( inByte < R4_START ) {
            controllerState.rightJoystickRadius = 3;
            controllerState.rightJoystickAngle  = (float)( inByte - R3_START ) / ( R4_START - R3_START );
        } else if ( inByte < R5_START ) {
            controllerState.rightJoystickRadius = 4;
            controllerState.rightJoystickAngle  = (float)( inByte - R4_START ) / ( R5_START - R4_START );
        } else if ( inByte < R6_START ) {
            controllerState.rightJoystickRadius = 5;
            controllerState.rightJoystickAngle  = (float)( inByte - R5_START ) / ( R6_START - R5_START );
        } else if ( inByte < R7_START ) {
            controllerState.rightJoystickRadius = 6;
            controllerState.rightJoystickAngle  = (float)( inByte - R6_START ) / ( R7_START - R6_START );
        } else {
            controllerState.rightJoystickRadius = 7;
            controllerState.rightJoystickAngle  = (float)( inByte - R7_START ) / ( 0x100 - R7_START );
        }
        if ( controllerState.rightJoystickAngle > 0.5 ) {
            controllerState.rightJoystickAngle -= 1;
        }
        controllerState.rightJoystickAngle *= 2 * M_PI;
    }
    
    if ( header & (1<<L_JOYSTICK) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        if ( inByte < R1_START ) {
            controllerState.leftJoystickRadius = 0;
            controllerState.leftJoystickAngle  = 0;
        } else if ( inByte < R2_START ) {
            controllerState.leftJoystickRadius = 1;
            controllerState.leftJoystickAngle  = (float)( inByte - R1_START ) / ( R2_START - R1_START );
        } else if ( inByte < R3_START ) {
            controllerState.leftJoystickRadius = 2;
            controllerState.leftJoystickAngle  = (float)( inByte - R2_START ) / ( R3_START - R2_START );
        } else if ( inByte < R4_START ) {
            controllerState.leftJoystickRadius = 3;
            controllerState.leftJoystickAngle  = (float)( inByte - R3_START ) / ( R4_START - R3_START );
        } else if ( inByte < R5_START ) {
            controllerState.leftJoystickRadius = 4;
            controllerState.leftJoystickAngle  = (float)( inByte - R4_START ) / ( R5_START - R4_START );
        } else if ( inByte < R6_START ) {
            controllerState.leftJoystickRadius = 5;
            controllerState.leftJoystickAngle  = (float)( inByte - R5_START ) / ( R6_START - R5_START );
        } else if ( inByte < R7_START ) {
            controllerState.leftJoystickRadius = 6;
            controllerState.leftJoystickAngle  = (float)( inByte - R6_START ) / ( R7_START - R6_START );
        } else {
            controllerState.leftJoystickRadius = 7;
            controllerState.leftJoystickAngle  = (float)( inByte - R7_START ) / ( 0x100 - R7_START );
        }
        if ( controllerState.leftJoystickAngle > 0.5 ) {
            controllerState.leftJoystickAngle -= 1;
        }
        controllerState.leftJoystickAngle *= 2 * M_PI;
    }
    
    if ( header & (1<<TRIGGERS) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        controllerState.rightTrigger = inByte & 0x0F;
        controllerState.leftTrigger  = inByte >> 4;
    }
    
    if ( header & (1<<DPAD) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        controllerState.dPadUp    = ( inByte & (1<<DPAD_UP)    );
        controllerState.dPadDown  = ( inByte & (1<<DPAD_DOWN)  );
        controllerState.dPadRight = ( inByte & (1<<DPAD_RIGHT) );
        controllerState.dPadLeft  = ( inByte & (1<<DPAD_LEFT)  );
    }
    
    if ( header & (1<<BUTTONS) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        controllerState.buttonA = ( inByte & (1<<BUTTON_A) );
        controllerState.buttonB = ( inByte & (1<<BUTTON_B) );
        controllerState.buttonX = ( inByte & (1<<BUTTON_X) );
        controllerState.buttonY = ( inByte & (1<<BUTTON_Y) );
        controllerState.rightBumper = ( inByte & (1<<R_BUMPER) );
        controllerState.leftBumper  = ( inByte & (1<<L_BUMPER) );
        controllerState.select = ( inByte & (1<<SELECT) );
        controllerState.start  = ( inByte & (1<<START)  );
    }
    
    return true;
}

void Bluetooth::waitForSerial() {
    while ( !btSerial->available() ) {
        idle();
    }
}

void Bluetooth::sendState() {
    uint32_t ms = millis();
    if ( ms - lastSendTime < sendCooldown ) return;
    
    
    
    
}
