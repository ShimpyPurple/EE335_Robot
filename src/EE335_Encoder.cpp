#include "EE335_Encoder.h"

Encoder::Encoder( uint8_t pin , uint8_t timer , uint8_t thread , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    thread( thread ) ,
    wheelCircumference( wheelCircumference ) ,
    holesPerRevolution( holesPerRevolution ) ,
    period( UINT16_MAX ) ,
    timer( new GenericTimer(timer , true) )
{}

Encoder::Encoder( uint8_t pin , BaseTimer16 *timer16 , uint8_t thread , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    thread( thread ) ,
    wheelCircumference( wheelCircumference ) ,
    holesPerRevolution( holesPerRevolution ) ,
    period( UINT16_MAX ) ,
    timer( new GenericTimer(timer16) )
{}

Encoder::Encoder( uint8_t pin , BaseTimer8Async *timer8 , uint8_t thread , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    thread( thread ) ,
    wheelCircumference( wheelCircumference ) ,
    holesPerRevolution( holesPerRevolution ) ,
    period( UINT16_MAX ) ,
    timer( new GenericTimer(timer8 , true) )
{}

Encoder::Encoder( uint8_t pin , GenericTimer *timer , uint8_t thread , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    thread( thread ) ,
    wheelCircumference( wheelCircumference ) ,
    holesPerRevolution( holesPerRevolution ) ,
    period( UINT16_MAX ) ,
    timer( timer )
{}

void Encoder::begin() {
    pinMode( pin , INPUT );
    
    timer->setMode( WGM_NORMAL );
    timer->setClockSource( CLOCK_1024 );
    
    factor = timer->getTickRate() * wheelCircumference / holesPerRevolution;
    
    attachInterruptCustom( pin , RISING , onRisingEdge , this );
    switch ( thread ) {
        case THREAD_A: timer->attachInterrupt( COMPARE_MATCH_A , onWrap , this ); break;
        case THREAD_B: timer->attachInterrupt( COMPARE_MATCH_B , onWrap , this ); break;
#if defined( __AVR_ATmega2560__ )
        case THREAD_C: timer->attachInterrupt( COMPARE_MATCH_C , onWrap , this ); break;
#endif
    }
    
}

float Encoder::getSpeed() {
    if ( period == UINT16_MAX ) return 0;
    return factor / period;
}

static void Encoder::onRisingEdge( void *object ) {
    Encoder *encoder = ( Encoder* )( object );
    GenericTimer *timer = encoder->timer;
    
    uint8_t interrupt;
    uint16_t (GenericTimer::*getOutputCompare)() = nullptr;
    void (GenericTimer::*setOutputCompare)( uint16_t ) = nullptr;
    
    switch ( encoder->thread ) {
        case THREAD_A: interrupt = COMPARE_MATCH_A; getOutputCompare = &timer->getOutputCompareA; setOutputCompare = &timer->setOutputCompareA; break;
        case THREAD_B: interrupt = COMPARE_MATCH_B; getOutputCompare = &timer->getOutputCompareB; setOutputCompare = &timer->setOutputCompareB; break;
#if defined( __AVR_ATmega2560__ )
        case THREAD_C: interrupt = COMPARE_MATCH_C; getOutputCompare = &timer->getOutputCompareC; setOutputCompare = &timer->setOutputCompareC; break;
#endif
        default: return;
    }
    
    
    if ( timer->interruptEnabled(interrupt) ) {
        uint16_t newPeriod = timer->getCounter() - ( timer->*getOutputCompare )();
        if ( newPeriod > 5 ) {
            encoder->period = newPeriod;
        }
    } else {
        timer->enableInterrupt( interrupt );
    }
    ( timer->*setOutputCompare )( timer->getCounter() - 1 );
}

static void Encoder::onWrap( void *object ) {
    Encoder *encoder = ( Encoder* )( object );
    encoder->period = UINT16_MAX;
    
    switch ( encoder->thread ) {
        case THREAD_A: encoder->timer->disableInterrupt( COMPARE_MATCH_A ); break;
        case THREAD_B: encoder->timer->disableInterrupt( COMPARE_MATCH_B ); break;
#if defined( __AVR_ATmega2560__ )
        case THREAD_C: encoder->timer->disableInterrupt( COMPARE_MATCH_C ); break;
#endif
    }
}
