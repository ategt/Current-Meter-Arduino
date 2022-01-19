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
            char *p_testFrequency, *p_intercept, *p_slope, *p_voltage, *p_printPeriod;
            char *token;
            char *end;
            unsigned int i = 0;

            token = strtok(serialInputString, ",");

            while( token != NULL ) {
              switch (i) {
                case 0:
                  {
                    p_testFrequency = token;
                  }
                  break;
                case 1:
                  {
                    p_intercept = token;
                  }
                  break;
                case 2:
                  {
                    p_slope = token;
                  }
                  break;
                case 3:
                  {
                    p_voltage = token;
                  }
                  break;
                case 4:
                  {
                    p_printPeriod = token;
                  }
                  break;
                default:
                  break;
              }

              token = strtok(NULL, ",");
              i++;
            }

            if (p_testFrequency == NULL || p_intercept == NULL || p_slope == NULL || p_voltage == NULL || p_printPeriod == NULL)
            {
              cmd = CMD_ERR;
            }
            else
            {
              args.configArgs.testFrequency = atof(p_testFrequency);
              args.configArgs.intercept = atof(p_intercept);
              args.configArgs.slope = atof(p_slope);
              args.configArgs.voltage = atof(p_voltage);
              args.configArgs.printPeriod = strtoul(p_printPeriod, &end, 10);
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
      else if (strcmp("rstconf", serialInputString) == 0)
      {
        cmd = CMD_RESET_CONFIG;
      }
      else if (strcmp("readconf", serialInputString) == 0)
      {
        cmd = CMD_READ_CONFIG;
      }
      else if (strcmp("readraw", serialInputString) == 0)
      {
        cmd = CMD_READ_RAW;
      }
      else if (strcmp("readdelay", serialInputString) == 0)
      {
        cmd = CMD_READ_DELAY;
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
