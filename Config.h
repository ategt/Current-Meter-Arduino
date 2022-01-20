#include <avr/eeprom.h>

const char nameAndVersion[] = "Current Meter V1.0";

struct Config {
  char appNameAndVersion[sizeof(nameAndVersion)];
  // Configuration parameters.
  float testFrequency;    // test signal frequency (Hz)
  float intercept;        // to be adjusted based on calibration testing
  float slope;            // to be adjusted based on calibration testing
  float voltage;
  float windowLength;     // used in noise reduction formula
  unsigned long printPeriod; // in milliseconds

  void save()
  {
    eeprom_write_block(this, (void *)0, sizeof (Config));
  }

  void load()
  {
    // Attempt to load config from EEPROM
    eeprom_read_block(this, (void *)0, sizeof (Config));
  
    if (strcmp(this->appNameAndVersion, nameAndVersion) != 0)
    {
      // Config not found in eeprom, so set default values here.
      reset();
    }
  }


  void reset()
  {
    memset(this, 0, sizeof (Config));
    strcpy(this->appNameAndVersion, nameAndVersion);
    this->testFrequency = 50;
    this->intercept = 0;
    this->slope = 1;
    this->voltage = 120;
    this->windowLength = 1;
    this->printPeriod = 1000;
  }
};

Config currentConfig;
