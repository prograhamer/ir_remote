/*
 * ir_remote.h
 *
 */

#ifndef __IR_REMOTE_H_
#define __IR_REMOTE_H_

#include <Arduino.h>

class IrRemote
{
  public:
    IrRemote();
    ~IrRemote();
    void initialise( const uint8_t frequency );
    const void sendCommand( const int command[][2] );
  private:
    uint8_t freq_ticks;
    uint8_t duty_ticks;
    static const int IRledPin = 3; // OC2B
};

#define FREQ_CONV_FACTOR 8000

#endif
