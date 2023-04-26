#ifndef EE335_Encoder_h
#define EE335_Encoder_h

#include "Arduino.h"
#include "Constants/Constants.h"
#include "CustomTimers.h"
#include "CustomInterrupts.h"

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega2560__ )
#warning "EE335_Encoder is only tested for ATmega328P and ATmega2560"
#endif

class Encoder {
	public:
		Encoder( uint8_t pin , uint8_t timer , uint8_t thread , bool usePCInt , float wheelCircumference , uint8_t holesPerRevolution );
        float getSpeed();
	
	private:
        uint8_t pin;
        BaseTimer16 *timer;
        uint8_t thread;
        float factor;
        uint16_t period;
        static void onRisingEdge( void *object );
        static void onWrap( void *object );
		
};

#endif
