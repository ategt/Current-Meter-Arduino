#include <avr/eeprom.h>

const char nameAndVersion[] = "Android Speedometer V1.0";

struct Config {
  char appNameAndVersion[sizeof(nameAndVersion)];
  // Configuration parameters.
  unsigned int wheelDiameterMm; // millimetres
  float speedLimitKmh;   // trigger alarm if limit exceeded.

  // Persistent variables.
  unsigned long totalDistanceMetres;
  unsigned long totalTimeSeconds;
  unsigned long tripDistanceMetres;
  unsigned long tripTimeSeconds;
  float tripMaxSpeedKmh;


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
    this->wheelDiameterMm = 600;  // millimetres
    this->speedLimitKmh = 10;
    this->tripMaxSpeedKmh = 0;
  }
};

Config currentConfig;

