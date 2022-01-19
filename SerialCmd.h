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
  CMD_READ_RAW,
  CMD_READ_DELAY,
  CMD_RESET_CONFIG,
  CMD_READ_CONFIG,
  CMD_WRITE_CONFIG
};


struct ConfigArgs {
  float testFrequency;    // test signal frequency (Hz)
  float intercept;        // calibration base
  float slope;            // calibration adjustment
  float voltage;
  unsigned long printPeriod; // in milliseconds
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
