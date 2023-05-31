#include "EE335_Lineread.h"

Lineread::Lineread( uint8_t pin , void (*onLineEdge)(uint8_t) ):
    pin( pin ) ,
    onLineEdge( onLineEdge )
{}

void Lineread::begin(){
    pinMode( pin , INPUT );
    attachInterruptCustom(
        pin ,
        CHANGE ,
        []( void *object , uint8_t edgeType ){
            ( (Lineread*)object )->onLineEdge( edgeType );
        } ,
        this
    );
}
