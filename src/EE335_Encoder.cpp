#include "EE335_Encoder.h"

Encoder::Encoder( uint8_t pin , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    distancePerHole( wheelCircumference / holesPerRevolution ) ,
    distance( 0 ) ,
    lastEdge( 0 ) ,
    period( MAX_PERIOD )
{}

void Encoder::begin() {
    pinMode( pin , INPUT );
    attachInterruptCustom( pin , RISING , onRisingEdge , this );
}

float Encoder::getDistance() {
    return distance;
}

float Encoder::getSpeed() {
    if ( period >= MAX_PERIOD ) return 0;
    uint32_t us = micros();
    if ( us - lastEdge > period ) period = us - lastEdge;
    if ( period >= MAX_PERIOD ) return 0;
    return distancePerHole * 1e6 / period;
}

static void Encoder::onRisingEdge( void *object ) {
    Encoder *encoder = ( Encoder* )( object );
    uint32_t us = micros();
    encoder->distance += encoder->distancePerHole;
    encoder->period = us - encoder->lastEdge;
    encoder->lastEdge = us;
}
