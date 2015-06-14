
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <RTC_DS3231.h>

RTC_DS3231 RTC;


// easier to reference here...see .h file for more options
//#define SQW_FREQ DS3231_SQW_FREQ_1      //  0b00000000  1Hz
#define SQW_FREQ DS3231_SQW_FREQ_1024     //0b00001000   1024Hz
//#define SQW_FREQ DS3231_SQW_FREQ_4096  // 0b00010000   4096Hz
//#define SQW_FREQ DS3231_SQW_FREQ_8192 //0b00011000      8192Hz

#define PWM_COUNT 1020   //determines how often the LED flips
#define LOOP_DELAY 5000 //ms delay time in loop

#define RTC_SQW_IN 5     // input square wave from RTC into T1 pin (D5)
                               //WE USE TIMER1 so that it does not interfere with Arduino delay() command
#define INT0_PIN   2     // INT0 pin for 32kHz testing?
#define LED_PIN    9     // random LED for testing...tie to ground through series resistor..
#define LED_ONBAORD 13   // Instead of hooking up an LED, the nano has an LED at pin 13.

#define SD_CHIP_SELECT 4

//----------- GLOBALS  -------------------------

volatile long TOGGLE_COUNT = 0;

// time (ms) between taking measurements
#define TIME_BETWEEN_MEASUREMENTS 10000

//####################################################################################
// SETUP
//####################################################################################
void setup () {
  //Serial.begin(57600);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  analogReference(EXTERNAL);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  
  pinMode(RTC_SQW_IN, INPUT);
  pinMode(INT0_PIN, INPUT);
      
  //-----------SD SETUP--------  
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  //--------RTC SETUP ------------
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    //Serial.println("RTC is older than compile time!  Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  RTC.enable32kHz(true);
  RTC.SQWEnable(true);
  RTC.BBSQWEnable(true);
  RTC.SQWFrequency( SQW_FREQ );

  char datastr[100];
  RTC.getControlRegisterData( datastr[0]  );
  Serial.print(  datastr );
 


  //--------INT 0---------------
  EICRA = 0;      //clear it
  EICRA |= (1 << ISC01);
  EICRA |= (1 << ISC00);   //ISC0[1:0] = 0b11  rising edge INT0 creates interrupt
  EIMSK |= (1 << INT0);    //enable INT0 interrupt
      
  //--------COUNTER 1 SETUP -------
  setupTimer1ForCounting((int)PWM_COUNT); 
  printTimer1Info();   
}


//####################################################################################
// MAIN
//####################################################################################
void loop () {
  /*
  Serial.print("Toggle Count over ");
  Serial.print(LOOP_DELAY, DEC);
  Serial.print("ms with PWM_COUNT of ");
  Serial.print(PWM_COUNT, DEC);
  Serial.print(":  ");
  Serial.print(TOGGLE_COUNT, DEC);
  Serial.println();
  */
  TOGGLE_COUNT = 0;

  DateTime now = RTC.now();
  
  RTC.forceTempConv(true);  //DS3231 does this every 64 seconds, we are simply testing the function here
  float temp_float = RTC.getTempAsFloat();
  int16_t temp_word = RTC.getTempAsWord();
  int8_t temp_hbyte = temp_word >> 8;
  int8_t temp_lbyte = temp_word &= 0x00FF;
  
  
  
  Serial.print(now.year(), DEC);
  Serial.print('-');
  Serial.print(now.month(), DEC);
  Serial.print('-');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" -- Clock T: ");
  Serial.print(temp_hbyte, DEC);
  Serial.print(".");
  Serial.print(temp_lbyte, DEC);
  Serial.print(" *C");
  Serial.println();
  
       
  Serial.print("Avg Temp "); 
  Serial.print(temp(A0));
  Serial.println(" *C");
  
  Serial.println();

  String dataString = "";
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    float sensor = temp(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  
  //delay(LOOP_DELAY);
   
  delay(TIME_BETWEEN_MEASUREMENTS);
}

