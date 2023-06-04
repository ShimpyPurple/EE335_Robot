#include "EE335_Bluetooth.h"

#define R_JOYSTICK 0
#define L_JOYSTICK 1
#define R1_START 0x04
#define R2_START 0x10
#define R3_START 0x24
#define R4_START 0x40
#define R5_START 0x64
#define R6_START 0x90
#define R7_START 0xC4

#define TRIGGERS 2

#define DPAD 3
#define DPAD_UP    0
#define DPAD_DOWN  1
#define DPAD_RIGHT 2
#define DPAD_LEFT  3

#define BUTTONS 4
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define R_BUMPER 4
#define L_BUMPER 5
#define SELECT 6
#define START  7

// ----------------------------------- //
//            Idle Function            //
// ----------------------------------- //

void idle() __attribute__( (weak) );

// -------------------------------------- //
//            ControllerButton            //
// -------------------------------------- //

ControllerButton::ControllerButton():
    state( false ) ,
    prev( false )
{}

void ControllerButton::setState( bool newState ) {
    prev = state;
    state = newState;
}

bool ControllerButton::getState() { return state; }
bool ControllerButton::isPressed()  { return  state && !prev; }
bool ControllerButton::isReleased() { return !state &&  prev; }

// ------------------------------- //
//            Bluetooth            //
// ------------------------------- //

Bluetooth::Bluetooth( uint8_t serialPort , uint8_t statePin , uint16_t sendCooldown ):
    connected( false ) ,
    statePin( statePin ) ,
    sendCooldown( sendCooldown ) ,
    lastSendTime( 0 )
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
}

void Bluetooth::begin() {
    btSerial->begin( 9600 );
    pinMode( statePin , INPUT );
}

bool Bluetooth::getInstruction() {
    bool currentState = digitalRead( statePin );
    if( !currentState ) {
        if ( connected != currentState ) {
            connected = false;
            
            controllerState.rightJoystickRadius = 0;
            controllerState.leftJoystickRadius  = 0;
            controllerState.rightJoystickAngle = 0;
            controllerState.leftJoystickAngle  = 0;
            controllerState.rightTrigger = 0;
            controllerState.leftTrigger  = 0;
            controllerState.dPadUp.setState(    false );
            controllerState.dPadDown.setState(  false );
            controllerState.dPadRight.setState( false );
            controllerState.dPadLeft.setState(  false );
            controllerState.buttonA.setState( false );
            controllerState.buttonB.setState( false );
            controllerState.buttonX.setState( false );
            controllerState.buttonY.setState( false );
            controllerState.rightBumper.setState( false );
            controllerState.leftBumper.setState(  false );
            controllerState.select.setState( false );
            controllerState.start.setState(  false );
            
            return true;
        }
        return false;
    }
    connected = true;
    
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
        controllerState.dPadUp.setState(    inByte & (1<<DPAD_UP)    );
        controllerState.dPadDown.setState(  inByte & (1<<DPAD_DOWN)  );
        controllerState.dPadRight.setState( inByte & (1<<DPAD_RIGHT) );
        controllerState.dPadLeft.setState(  inByte & (1<<DPAD_LEFT)  );
    }
    
    if ( header & (1<<BUTTONS) ) {
        waitForSerial();
        uint8_t inByte = btSerial->read();
        controllerState.buttonA.setState( inByte & (1<<BUTTON_A) );
        controllerState.buttonB.setState( inByte & (1<<BUTTON_B) );
        controllerState.buttonX.setState( inByte & (1<<BUTTON_X) );
        controllerState.buttonY.setState( inByte & (1<<BUTTON_Y) );
        controllerState.rightBumper.setState( inByte & (1<<R_BUMPER) );
        controllerState.leftBumper.setState(  inByte & (1<<L_BUMPER) );
        controllerState.select.setState( inByte & (1<<SELECT) );
        controllerState.start.setState(  inByte & (1<<START)  );
    }
    
    return true;
}

void Bluetooth::waitForSerial() {
    while ( !btSerial->available() ) {
        idle();
    }
}

void Bluetooth::sendState( void (*updateState)() ) {
    uint32_t ms = millis();
    if ( ms - lastSendTime < sendCooldown ) return;
    
    if ( !digitalRead(statePin) ) return;
    
    uint8_t oldSREG = SREG;
    cli();
    updateState();
    SREG = oldSREG;
    
    uint8_t numBytesToSend = 1;
    uint8_t buffer[5] = { 0,0,0,0,0 };
    
    if ( speedometerState.speed != prevSpeedometerState.speed ) {
        prevSpeedometerState.speed = speedometerState.speed;
        buffer[numBytesToSend] = speedometerState.speed;
        buffer[0] |= 0x01;
        numBytesToSend += 1;
    }
    
    if ( speedometerState.cruiseSpeed != prevSpeedometerState.cruiseSpeed ) {
        prevSpeedometerState.cruiseSpeed = speedometerState.cruiseSpeed;
        buffer[numBytesToSend] = speedometerState.cruiseSpeed;
        buffer[0] |= 0x02;
        numBytesToSend += 1;
    }
    
    if ( speedometerState.cruiseControl ) buffer[0] |= 0x04;
    if ( speedometerState.lineFollowing ) buffer[0] |= 0x08;
    
    if ( sonarState.heading != prevSonarState.heading ) {
        if ( sonarState.heading != ALL_HEADINGS ) {
            prevSonarState.heading = sonarState.heading;            
        }
        buffer[numBytesToSend] = sonarState.heading;
        buffer[0] |= 0x10;
        numBytesToSend += 1;
    }
    
    if ( sonarState.range != prevSonarState.range ) {
        if ( sonarState.heading != ALL_HEADINGS ) {
            prevSonarState.range = sonarState.range;            
        }
        buffer[numBytesToSend] = sonarState.range;
        buffer[0] |= 0x20;
        numBytesToSend += 1;
    }
    
    if (
        ( speedometerState.cruiseControl != prevSpeedometerState.cruiseControl ) ||
        ( speedometerState.lineFollowing != prevSpeedometerState.lineFollowing ) ||
        ( numBytesToSend > 1 )
    ) {
        prevSpeedometerState.cruiseControl = speedometerState.cruiseControl;
        prevSpeedometerState.lineFollowing = speedometerState.lineFollowing;
        
        btSerial->write( buffer , numBytesToSend );
        
        lastSendTime = ms;
    }
}
