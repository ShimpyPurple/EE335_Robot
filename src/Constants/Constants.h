#include "Arduino.h"

// Serial sources
#define SERIAL_0 0
#define SERIAL_1 1
#define SERIAL_2 2
#define SERIAL_3 3

// Bluetooth rx modes
#define CONTROLLER 0
#define PLAIN_TEXT 1

// Game Controller
#define R_JOYSTICK 0
#define L_JOYSTICK 1
#define TRIGGERS   2
#define GAMEPAD    3
#define OTHER      4
#define CHECKBIT   7
#define R1_START 0x04
#define R2_START 0x10
#define R3_START 0x24
#define R4_START 0x40
#define R5_START 0x64
#define R6_START 0x90
#define R7_START 0xC4
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define DPAD_UP    4
#define DPAD_DOWN  5
#define DPAD_RIGHT 6
#define DPAD_LEFT  7
#define R_BUMPER 0
#define L_BUMPER 1
#define SELECT 2
#define START  3
#define MODE   4

// Timer Threads
#define THREAD_A 0
#define THREAD_B 1
#if defined( __AVR_ATmega2560__ )
#define THREAD_C 2
#endif
