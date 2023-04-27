#include "EE335_Encoder.h"

Encoder::Encoder( uint8_t pin , uint8_t timer , uint8_t thread , bool usePCInt , float wheelCircumference , uint8_t holesPerRevolution ):
    pin( pin ) ,
    thread( thread )
{
    period = UINT16_MAX;
    
    switch ( timer ) {
        case TIMER_1: this->timer = &Timer1; break;
#if defined( __AVR_ATmega2560__ )
        case TIMER_3: this->timer = &Timer3; break;
        case TIMER_4: this->timer = &Timer4; break;
        case TIMER_5: this->timer = &Timer5; break;
#endif
        default: timerReserved = false; return;
    }
    
    if ( this->timer->isFree() ) {
        this->timer->reserve();
        timerReserved = true;
    } else {
        timerReserved = false;
        return;
    }
    
    this->timer->setMode( NORMAL );
    this->timer->setClockSource( CLOCK_1024 );
    
    factor = this->timer->getTickRate() * wheelCircumference / holesPerRevolution;
    
    attachInterruptCustom( pin , RISING , onRisingEdge , this );
    switch ( thread ) {
        case THREAD_A: this->timer->attachInterrupt( COMPARE_MATCH_A , onWrap , this ); break;
        case THREAD_B: this->timer->attachInterrupt( COMPARE_MATCH_B , onWrap , this ); break;
#if defined( __AVR_ATmega2560__ )
        case THREAD_C: this->timer->attachInterrupt( COMPARE_MATCH_C , onWrap , this ); break;
#endif
    }
    
}

float Encoder::getSpeed() {
    return factor / period;
}

void Encoder::kill() {
    if ( timerReserved ) {
        timer->release();
    }
}

static void Encoder::onRisingEdge( void *object ) {
    Encoder *encoder = ( Encoder* )( object );
    BaseTimer16 *timer = encoder->timer;
    
    uint8_t interrupt;
    uint16_t (BaseTimer16::*getOutputCompare)() = nullptr;
    void (BaseTimer16::*setOutputCompare)( uint16_t ) = nullptr;
    
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
