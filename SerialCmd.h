#ifndef SERIALCMD_H
#define SERIALCMD_H

#include "Arduino.h"

#if defined(__AVR_ATmega328P__)
  #define MySerial  Serial
#elif defined(__AVR_ATmega32U4__)
  #define MySerial  Serial1
#elif defined(__AVR_ATmega2560__)
  #define MySerial  Serial
#endif


enum SerialCmdValues
{
  CMD_ERR = 1,
  CMD_READ_DATA,
  CMD_RESET_TRIP,
  CMD_READ_CONFIG,
  CMD_WRITE_CONFIG
};


struct ConfigArgs {
  unsigned int wheelDiameterMm;
  float speedLimitKmh;
};

union SerialCommandArgs {
  ConfigArgs configArgs;
};


class SerialCommand {
  public:
    byte cmd;
    SerialCommandArgs args;
    byte complete;
  
    void reset();
    void serialEvent();

  private:
    char serialInputString[32] = "";
    byte inStrIdx =0;
};

extern SerialCommand serialCmd;

#endif
