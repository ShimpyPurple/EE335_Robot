#include "EE335_Lineread.h"

Lineread::Lineread( uint8_t rightPin , uint8_t leftPin , void (*onLineEdge)(uint8_t , uint8_t) ):
    rightPin( rightPin ) ,
    leftPin( leftPin ) ,
    onLineEdge( onLineEdge ) ,
    following( false )
{}

void Lineread::begin(){
    
    pinMode( rightPin , INPUT );
    attachInterruptCustom(
        rightPin ,
        CHANGE ,
        []( void *object , uint8_t edgeType ){
            Lineread *lineread = ( Lineread* )( object );
            if ( lineread->following ) {
                lineread->onLineEdge( edgeType , LINEREAD_RIGHT );
            }
        } ,
        this
    );
    
    pinMode( leftPin , INPUT );
    attachInterruptCustom(
        leftPin ,
        CHANGE ,
        []( void *object , uint8_t edgeType ){
            Lineread *lineread = ( Lineread* )( object );
            if ( lineread->following ) {
                lineread->onLineEdge( edgeType , LINEREAD_LEFT );
            }
        } ,
        this
    );
    
}

void Lineread::startFollowing() { following = true; }
void Lineread::stopFollowing() { following = false; }
bool Lineread::isFollowing() { return following; }
