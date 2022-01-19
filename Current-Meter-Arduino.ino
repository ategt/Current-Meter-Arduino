/* This code works with ACS712 current sensor, it permits the calculation of the signal TRMS
 * Visit www.surtrtech.com for more details
 */
#include <Filters.h>                      //This library does a massive work check it's .cpp file

#include "SerialCmd.h"
#include "Config.h"

#define ACS_Pin A0                        //Sensor data pin on A0 analog input

float ACS_Value;                              //Here we keep the raw data valuess
float windowLength;     // how long to average the signal, for statistist
float Amps_TRMS; // estimated actual current in amps
float adjusted_amps_TRMS;

// Track time in milliseconds since last reading 
unsigned long previousMillis = 0;
byte readingActive = false;

void setup() {
  MySerial.begin( 9600 );    // Start the serial port
  pinMode(ACS_Pin,INPUT);  //Define the pin mode

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  currentConfig.load();

  windowLength = 40.0/currentConfig.testFrequency;
}

void takeReading() {
  readingActive = true;
  RunningStatistics inputStats;                 // create statistics to look at the raw test signal
  inputStats.setWindowSecs( currentConfig.printPeriod / 1000 );     //Set the window length

  while( true ) {
    ACS_Value = analogRead(ACS_Pin);  // read the analog in value:
    inputStats.input(ACS_Value);  // log to Stats function
        
    if((unsigned long)(millis() - previousMillis) >= currentConfig.printPeriod) { //every second we do the calculation
      previousMillis = millis();   // update time
      
      Amps_TRMS = inputStats.sigma();
      adjusted_amps_TRMS = (Amps_TRMS + currentConfig.intercept) * currentConfig.slope;

      MySerial.print( "\t Reading: " ); 
      MySerial.print( Amps_TRMS );

      MySerial.print( "\t Amps: " ); 
      MySerial.print( adjusted_amps_TRMS );

      MySerial.print( "\t Watts: " ); 
      MySerial.println( adjusted_amps_TRMS * currentConfig.voltage );

      break;
    }
  }

  readingActive = false;
}

void loop() {
  serialCmd.serialEvent();

  // Process serial commands.
  if (serialCmd.complete)
  {
    switch (serialCmd.cmd)
    {
      case CMD_READ_DATA:
        if ( !readingActive ) {
          takeReading();
        } else {
          MySerial.println("overlap");
        }
        break;
      case CMD_RESET_CONFIG:
        currentConfig.reset();
        currentConfig.save();

        windowLength = 40.0/currentConfig.testFrequency;

        MySerial.println("ok");
        break;
      case CMD_READ_CONFIG:
        MySerial.print((unsigned int)currentConfig.testFrequency);
        MySerial.print(",");
        MySerial.print(currentConfig.intercept);
        MySerial.print(",");
        MySerial.print(currentConfig.slope);
        MySerial.print(",");
        MySerial.print((unsigned int)currentConfig.voltage);
        MySerial.print(",");
        MySerial.println(currentConfig.printPeriod);

        break;
      case CMD_WRITE_CONFIG:
        currentConfig.testFrequency = constrain(serialCmd.args.configArgs.testFrequency, 1, 1000);
        currentConfig.intercept = serialCmd.args.configArgs.intercept;
        currentConfig.slope = serialCmd.args.configArgs.slope;
        currentConfig.voltage = constrain(serialCmd.args.configArgs.voltage, 1, 1000);
        currentConfig.printPeriod = constrain(serialCmd.args.configArgs.printPeriod, 1, 1000000);
        currentConfig.save();
   
        windowLength = 40.0/currentConfig.testFrequency;

        MySerial.println("ok");

        break;
      case CMD_READ_RAW:
        MySerial.print(millis());
        MySerial.print(",");

        for ( unsigned int i = 0; i < currentConfig.printPeriod ; i++ ) {
          MySerial.print(analogRead(ACS_Pin));
          MySerial.print(",");
        }

        MySerial.print(analogRead(ACS_Pin));
        MySerial.print(",");
        MySerial.println(millis());

        MySerial.println("ok");

        break;
      case CMD_READ_DELAY:
        if((unsigned long)(millis() - previousMillis) < currentConfig.printPeriod) {
          MySerial.print("...pausing for ");
          MySerial.print(currentConfig.printPeriod - ((unsigned long)(millis() - previousMillis)));
          MySerial.println(" milliseconds");

          delay(currentConfig.printPeriod - ((unsigned long)(millis() - previousMillis)));
        }
        
        MySerial.print(millis());
        MySerial.print(",");
        MySerial.println(analogRead(ACS_Pin));

        previousMillis = millis();

        MySerial.println("ok");

        break;
      case CMD_ERR:
        MySerial.println("err");
        break;
    }
    serialCmd.reset();
  }
}

/* About the slope and intercept
 * First you need to know that all the TRMS calucations are done by functions from the library, it's the "inputStats.sigma()" value
 * At first you can display that "inputStats.sigma()" as your TRMS value, then try to measure using it when the input is 0.00A
 * If the measured value is 0 like I got you can keep the intercept as 0, otherwise you'll need to add or substract to make that value equal to 0
 * In other words " remove the offset"
 * Then turn on the power to a known value, for example use a bulb or a led that ou know its power and you already know your voltage, so a little math you'll get the theoritical amps
 * you divide that theory value by the measured value and here you got the slope, now place them or modify them
 */
