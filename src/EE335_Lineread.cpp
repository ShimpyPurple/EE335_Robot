#include "EE335_Lineread.h"

Lineread::Lineread( uint8_t pin ):
    pin( pin ) ,
    previnst( 0 )
{}

void Lineread::begin(){
    pinMode( pin , INPUT );
    attachInterruptCustom( pin , CHANGE , lineread_ISR , this );
}

void Lineread::lineread_ISR( void *object ){
    Lineread *lineread = ( Lineread* )( object );
    
    if( millis() - lineread->previnst <= 100 ){
        return;
    }
    lineread->previnst = millis();
    
    delay( 5 );
    
    if( digitalRead(lineread->pin) ){
        Serial.println( "LINE" );
    } else {
        Serial.println( "NO LINE" );
    }
}