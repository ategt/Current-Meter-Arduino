#include "SerialCmd.h"

SerialCommand serialCmd;

void SerialCommand::reset()
{
  cmd = 0;
  complete = false;
  memset(&args, 0, sizeof(SerialCommandArgs));
}

//----------------------------------------------------------------------
void SerialCommand::serialEvent()
{
  while (!complete && MySerial.available())
  {
    // get the next byte:
    char inChar = (char)MySerial.read();

    if (inChar == 127 && inStrIdx > 0)  // Deal with backspace key.
    {
      inStrIdx--;
      serialInputString[inStrIdx] = 0;
      continue;
    }
    else if (inChar == 3)               // Ctrl-c clears input string.
    {
      inStrIdx = 0;
      serialInputString[0] = 0;
      cmd = 0;
      continue;
    }
    
    char indexAdjust = 1;
    if (inStrIdx < sizeof (serialInputString) - 1)
    {
      serialInputString[inStrIdx++] = inChar;
      serialInputString[inStrIdx] = 0;
    }
    else
    {
      // input string overflow
      indexAdjust = 0;
    }

    if (cmd && (inChar == '\r' || inChar == '\n'))
    {
      inStrIdx -= indexAdjust;
      serialInputString[inStrIdx] = 0;  // trim away the trailing carriage return char.

      switch (cmd)
      {
        // Parse arguments
        case CMD_WRITE_CONFIG:
          {
            char *p_wheelDiameter, *p_speedLimit;
            p_wheelDiameter = strtok(serialInputString, ",");
            p_speedLimit = strtok((char *)0, "\r");   // "/r" not actually needed

            if (p_wheelDiameter == NULL || p_speedLimit == NULL)
            {
              cmd = CMD_ERR;
            }
            else
            {
              args.configArgs.wheelDiameterMm = atof(p_wheelDiameter) * 10;
              args.configArgs.speedLimitKmh = atof(p_speedLimit);
            }
          }
          break;
        default:
          break;
      }
      
      serialInputString[0] = 0;
      inStrIdx=0;
      complete = true;
    }
    else if (!cmd && inChar == ' ')   // commands expecting arguments.
    {
      inStrIdx -= indexAdjust;
      serialInputString[inStrIdx] = 0;  // trim away the trailing space char.
      complete = false;

      if (strcmp("writeconf", serialInputString) == 0)
      {
        cmd = CMD_WRITE_CONFIG;
      }
      else
      {
        cmd = CMD_ERR;
      }
      serialInputString[0] = 0;
      inStrIdx=0;
    }
    else if (!cmd && (inChar == '\r' || inChar == '\n'))  // commands not expecting arguments.
    {
      inStrIdx -= indexAdjust;
      serialInputString[inStrIdx] = 0;  // trim away the trailing CR char.
      complete = true;
      
      if (strcmp("readdata", serialInputString) == 0)
      {
        cmd = CMD_READ_DATA;
      }
      else if (strcmp("rstrip", serialInputString) == 0)
      {
        cmd = CMD_RESET_TRIP;
      }
      else if (strcmp("readconf", serialInputString) == 0)
      {
        cmd = CMD_READ_CONFIG;
      }
      else
      {
        if (inStrIdx > 0)
        {
          cmd = CMD_ERR;
        }
        else
        {
          complete = false;
        }
      }
      serialInputString[0] = 0;
      inStrIdx=0;
    }
    else if ((inChar == '\r' || inChar == '\n'))
    {
      serialInputString[0] = 0;
      inStrIdx=0;
    }
  }
}
