/*
 * ir_remote.cpp
 *
 * Library for sending commands over IR remote control protocol.
 *
 * Commands are encoded as a two-dimensional array of timings in microseconds.
 * The first dimension indicates the sequence of the pulse in the commands and
 * the second dimension indicates whether the time is for the pulse being ON or
 * OFF (commands[i][0] indicates the time for pulse i for which the signal
 * should be ON, commands[i][1] indicates the time for which the signal should
 * be OFF). The array is terminated by a zero in the commands[i][1].
 *
 * E.g. commands[][2] = {{200, 50}, {100, 150}, {200, 0}};
 * would cause the signal to be ON for 200us, then OFF for 50us, ON for 100us,
 * OFF for 150us and then ON for 200us. The sequence terminates at this point.
 *
 * This implemenation uses phase correct PWM simply because the clock frequency
 * divided by two gives a good resolution for the frequency ranges used in IR
 * remote control protocols. It is possible to use fast PWM with a prescale of
 * 8 but this gives much less accurate definition of the frequencies around
 * 36-38KHz.
 *
 */

#include "Arduino.h"
#include "ir_remote.h"

IrRemote::IrRemote()
{

} // IrRemote::IrRemote

IrRemote::~IrRemote()
{

} // IrRemote::~IrRemote

void IrRemote::initialise( const uint8_t frequency_khz )
{
  // Determine freq_ticks and duty_ticks
  // Using phase correct PWM, so period is doubled,
  // converting from KHz so multiply by 1000
  freq_ticks = FREQ_CONV_FACTOR / frequency_khz;
  // Work on the basis of a 50% duty cycle
  duty_ticks = (freq_ticks * 2) / 3;

  pinMode( IRledPin, OUTPUT );
  digitalWrite( IRledPin, LOW );

  // Disable all timer 2 interrupts
  TIMSK2 &= ~((1<<OCIE2A) | (1<<OCIE2B) | (1<<TOIE2));
  // No prescale
  TCCR2B &= ~((1<<CS21) | (1<<CS22));
  TCCR2B |= (1<<CS20);
  // Set frequency
  OCR2A = freq_ticks;
  // Set duty-cycle
  OCR2B = duty_ticks;
  // Enable phase correct PWM, TOP at OCR2A (OCR2A determines frequency)
  TCCR2A &= ~(1<<WGM21);
  TCCR2A |= (1<<WGM20);
  TCCR2B |= (1<<WGM22);
  // Disable OC2A output (pin 11)
  TCCR2A &= ~((1<<COM2A0) | (1<<COM2A1));
  // Disable OC2B output (pin 3)
  TCCR2A &= ~((1<<COM2B0) | (1<<COM2B1));
} // IrRemote::initialise

const void IrRemote::sendCommand( const int command[][2] )
{
  uint8_t i;
  uint8_t finished;

  for( i = 0, finished = 0; ! finished; i++ )
  {
    // Enable output on OC2B
    TCCR2A |= (1<<COM2B1);
    delayMicroseconds( command[i][0] );
    // Disable output on OC2B
    TCCR2A &= ~(1<<COM2B1);

    if( command[i][1] == 0 )
      finished = 1;
    else
      delayMicroseconds( command[i][1] );
  }
} // IrRemote::sendCommand
