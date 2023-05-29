#include "EE335_Bluetooth.h"

Bluetooth::Bluetooth( uint8_t serialPort ) {
    switch ( serialPort ) {
        case SERIAL_0: btSerial = &Serial;  break;
#if defined( __AVR_ATmega2560__ )
        case SERIAL_1: btSerial = &Serial1; break;
        case SERIAL_2: btSerial = &Serial2; break;
        case SERIAL_3: btSerial = &Serial3; break;
#endif
        default: btSerial = &Serial;  break;
    }
}

void Bluetooth::begin() {
    btSerial->begin( 9600 );
}

void Bluetooth::getInstruction() {
    waitForSerial();
    
    uint8_t header = btSerial->read();
    
    if ( header & (1<<R_JOYSTICK) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        if ( inByte < R1_START ) {
            ControllerState.rightJoystickRadius = 0;
            ControllerState.rightJoystickAngle  = 0;
        } else if ( inByte < R2_START ) {
            ControllerState.rightJoystickRadius = 1;
            ControllerState.rightJoystickAngle  = (float)( inByte - R1_START ) / ( R2_START - R1_START );
        } else if ( inByte < R3_START ) {
            ControllerState.rightJoystickRadius = 2;
            ControllerState.rightJoystickAngle  = (float)( inByte - R2_START ) / ( R3_START - R2_START );
        } else if ( inByte < R4_START ) {
            ControllerState.rightJoystickRadius = 3;
            ControllerState.rightJoystickAngle  = (float)( inByte - R3_START ) / ( R4_START - R3_START );
        } else if ( inByte < R5_START ) {
            ControllerState.rightJoystickRadius = 4;
            ControllerState.rightJoystickAngle  = (float)( inByte - R4_START ) / ( R5_START - R4_START );
        } else if ( inByte < R6_START ) {
            ControllerState.rightJoystickRadius = 5;
            ControllerState.rightJoystickAngle  = (float)( inByte - R5_START ) / ( R6_START - R5_START );
        } else if ( inByte < R7_START ) {
            ControllerState.rightJoystickRadius = 6;
            ControllerState.rightJoystickAngle  = (float)( inByte - R6_START ) / ( R7_START - R6_START );
        } else {
            ControllerState.rightJoystickRadius = 7;
            ControllerState.rightJoystickAngle  = (float)( inByte - R7_START ) / ( 0x100 - R7_START );
        }
        if ( ControllerState.rightJoystickAngle > 0.5 ) {
            ControllerState.rightJoystickAngle -= 1;
        }
        ControllerState.rightJoystickAngle *= 2 * M_PI;
    }
    
    if ( header & (1<<L_JOYSTICK) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        if ( inByte < R1_START ) {
            ControllerState.leftJoystickRadius = 0;
            ControllerState.leftJoystickAngle  = 0;
        } else if ( inByte < R2_START ) {
            ControllerState.leftJoystickRadius = 1;
            ControllerState.leftJoystickAngle  = (float)( inByte - R1_START ) / ( R2_START - R1_START );
        } else if ( inByte < R3_START ) {
            ControllerState.leftJoystickRadius = 2;
            ControllerState.leftJoystickAngle  = (float)( inByte - R2_START ) / ( R3_START - R2_START );
        } else if ( inByte < R4_START ) {
            ControllerState.leftJoystickRadius = 3;
            ControllerState.leftJoystickAngle  = (float)( inByte - R3_START ) / ( R4_START - R3_START );
        } else if ( inByte < R5_START ) {
            ControllerState.leftJoystickRadius = 4;
            ControllerState.leftJoystickAngle  = (float)( inByte - R4_START ) / ( R5_START - R4_START );
        } else if ( inByte < R6_START ) {
            ControllerState.leftJoystickRadius = 5;
            ControllerState.leftJoystickAngle  = (float)( inByte - R5_START ) / ( R6_START - R5_START );
        } else if ( inByte < R7_START ) {
            ControllerState.leftJoystickRadius = 6;
            ControllerState.leftJoystickAngle  = (float)( inByte - R6_START ) / ( R7_START - R6_START );
        } else {
            ControllerState.leftJoystickRadius = 7;
            ControllerState.leftJoystickAngle  = (float)( inByte - R7_START ) / ( 0x100 - R7_START );
        }
        if ( ControllerState.leftJoystickAngle > 0.5 ) {
            ControllerState.leftJoystickAngle -= 1;
        }
        ControllerState.leftJoystickAngle *= 2 * M_PI;
    }
    
    if ( header & (1<<TRIGGERS) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        ControllerState.rightTrigger = inByte & 0x0F;
        ControllerState.leftTrigger  = inByte >> 4;
    }
    
    if ( header & (1<<DPAD) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        ControllerState.dPadUp    = ( inByte & (1<<DPAD_UP)    );
        ControllerState.dPadDown  = ( inByte & (1<<DPAD_DOWN)  );
        ControllerState.dPadRight = ( inByte & (1<<DPAD_RIGHT) );
        ControllerState.dPadLeft  = ( inByte & (1<<DPAD_LEFT)  );
    }
    
    if ( header & (1<<BUTTONS) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        ControllerState.buttonA = ( inByte & (1<<BUTTON_A) );
        ControllerState.buttonB = ( inByte & (1<<BUTTON_B) );
        ControllerState.buttonX = ( inByte & (1<<BUTTON_X) );
        ControllerState.buttonY = ( inByte & (1<<BUTTON_Y) );
        ControllerState.rightBumper = ( inByte & (1<<R_BUMPER) );
        ControllerState.leftBumper  = ( inByte & (1<<L_BUMPER) );
        ControllerState.select = ( inByte & (1<<SELECT) );
        ControllerState.start  = ( inByte & (1<<START)  );
    }
    
}

void Bluetooth::waitForSerial() {
    while ( !btSerial->available() );
}
