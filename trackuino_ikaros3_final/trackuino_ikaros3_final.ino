/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation.
#if ARDUINO < 22
#error "Oops! We need Arduino 22 or later"
#endif

// Trackuino custom libs
#include "aprs.h"
#include "ax25.h"
#include "buzzer.h"
#include "config.h"
#include "debug.h"
#include "gps.h"
#include "modem.h"
#include "radio.h"
#include "radio_hx1.h"
#include "radio_mx146.h"
#include "sensors.h"

// Arduino/AVR libs
#include <Wire.h>
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif
#include <avr/power.h>
#include <avr/sleep.h>
#define __PROG_TYPES_COMPAT__ 1 //added for progmem. needed?
#include "avr/pgmspace.h" //added for progmem

// Data Logger libs
/*
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
const int chipSelect = 10;
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
*/

//Various constants and variables
unsigned long next_tx_millis;
unsigned long next_tweet_millis;
//unsigned long next_data_millis;
//unsigned long last_data_millis;
PROGMEM const prog_char TWEET0[] = "";
PROGMEM const prog_char TWEET1[] = "";
PROGMEM const prog_char TWEET2[] = "";
PROGMEM const prog_char TWEET3[] = "";
PROGMEM const prog_char TWEET4[] = "";
PROGMEM const prog_char TWEET5[] = "Flight computer active";
PROGMEM const prog_char TWEET6[] = "";
PROGMEM const prog_char TWEET7[] = "";
PROGMEM const prog_char TWEET8[] = "";
PROGMEM const prog_char TWEET9[] = "";
PROGMEM const prog_char TWEET10[] = "";
PROGMEM const prog_char TWEET11[] = "We have liftoff.";
PROGMEM const prog_char TWEET12[] = "Higher than highest peak in CA";
PROGMEM const prog_char TWEET13[] = "Higher than Mt. Everest";
PROGMEM const prog_char TWEET14[] = "Flying higher than airliners";
PROGMEM const prog_char TWEET15[] = "Entering blackness of space";
PROGMEM const prog_char TWEET16[] = "Passed Ikaros I's peak";
PROGMEM const prog_char TWEET17[] = "Beat Ikaros II's record";
PROGMEM const prog_char TWEET18[] = "Higher than an SR-71";
PROGMEM const prog_char TWEET19[] = "100K ft goal reached!";
PROGMEM const prog_char TWEET20[] = "105K ft!";
PROGMEM const prog_char TWEET21[] = "110K ft!!";
PROGMEM const prog_char TWEET22[] = "115K ft!!!";
PROGMEM const prog_char TWEET23[] = "120K ft!!!!";
PROGMEM const prog_char TWEET24[] = "";
PROGMEM const prog_char TWEET25[] = "";
PROGMEM const prog_char TWEET26[] = "";
PROGMEM const prog_char TWEET27[] = "";
PROGMEM const prog_char TWEET28[] = "";
PROGMEM const prog_char TWEET29[] = "";
PROGMEM const prog_char TWEET30[] = "";
PROGMEM const prog_char TWEET31[] = "Falling back to earth...";
PROGMEM const prog_char TWEET32[] = "";
PROGMEM const prog_char TWEET33[] = "";
PROGMEM const prog_char TWEET34[] = "";
PROGMEM const prog_char TWEET35[] = "";
PROGMEM const prog_char TWEET36[] = "";
PROGMEM const prog_char TWEET37[] = "";
PROGMEM const prog_char TWEET38[] = "";
PROGMEM const prog_char TWEET39[] = "";
PROGMEM const prog_char TWEET40[] = "";
PROGMEM const prog_char TWEET41[] = "Dropping through 100K ft";
PROGMEM const prog_char TWEET42[] = "Plummeting through 75K ft";
PROGMEM const prog_char TWEET43[] = "Sinking through 50K ft";
PROGMEM const prog_char TWEET44[] = "Braking through 25K ft";
PROGMEM const prog_char TWEET45[] = "Buckle up & brace for impact";
PROGMEM const prog_char TWEET46[] = "";
PROGMEM const prog_char TWEET47[] = "";
PROGMEM const prog_char TWEET48[] = "";
PROGMEM const prog_char TWEET49[] = "";
PROGMEM const prog_char TWEET50[] = "";
PROGMEM const prog_char TWEET51[] = "Ikaros has landed.";
PROGMEM const prog_char TWEET52[] = "Landing survived";
PROGMEM const prog_char TWEET53[] = "Waiting to be found";

PROGMEM const char *TWEETS[] = 	   // change "string_table" name to suit
{   
  TWEET0,
  TWEET1,
  TWEET2,
  TWEET3,
  TWEET4,
  TWEET5,
  TWEET6,
  TWEET7,
  TWEET8,
  TWEET9,
  TWEET10,
  TWEET11,
  TWEET12,
  TWEET13,
  TWEET14,
  TWEET15,
  TWEET16,
  TWEET17,
  TWEET18,
  TWEET19,
  TWEET20,
  TWEET21,
  TWEET22,
  TWEET23,
  TWEET24,
  TWEET25,
  TWEET26,
  TWEET27,
  TWEET28,
  TWEET29,
  TWEET30,
  TWEET31,
  TWEET32,
  TWEET33,
  TWEET34,
  TWEET35,
  TWEET36,
  TWEET37,
  TWEET38,
  TWEET39,
  TWEET40,
  TWEET41,
  TWEET42,
  TWEET43,
  TWEET44,
  TWEET45,
  TWEET46,
  TWEET47,
  TWEET48,
  TWEET49,
  TWEET50,
  TWEET51,
  TWEET52,
  TWEET53

};

  int8_t FLIGHT_STATUS = 0;
//  int LAST_FLIGHT_STATUS = 0;
  int last_altitude = 0;  
  int slast_altitude = 0;
  int max_altitude = 0;

void disable_bod_and_sleep()
{
  /* This will turn off brown-out detection while
   * sleeping. Unfortunately this won't work in IDLE mode.
   * Relevant info about BOD disabling: datasheet p.44
   *
   * Procedure to disable the BOD:
   *
   * 1. BODSE and BODS must be set to 1
   * 2. Turn BODSE to 0
   * 3. BODS will automatically turn 0 after 4 cycles
   *
   * The catch is that we *must* go to sleep between 2
   * and 3, ie. just before BODS turns 0.
   */
  unsigned char mcucr;

  cli();
  mcucr = MCUCR | (_BV(BODS) | _BV(BODSE));
  MCUCR = mcucr;
  MCUCR = mcucr & (~_BV(BODSE));
  sei();
  sleep_mode();    // Go to sleep
}

void power_save()
{
  /* Enter power saving mode. SLEEP_MODE_IDLE is the least saving
   * mode, but it's the only one that will keep the UART running.
   * In addition, we need timer0 to keep track of time, timer 1
   * to drive the buzzer and timer2 to keep pwm output at its rest
   * voltage.
   */

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();

  digitalWrite(LED_PIN, LOW);
  sleep_mode();    // Go to sleep
  digitalWrite(LED_PIN, HIGH);
  
  sleep_disable();  // Resume after wake up
  power_all_enable();

}


//Begin check_status() low altitude test--This code seems to work--status 111 got to 73s
void check_status(int altitude)
{
char msg[32];

  if ((altitude > 1) && (FLIGHT_STATUS < 5) && (altitude>max_altitude))//tweet startup
 {
   FLIGHT_STATUS= 5;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[5])));
   tweet_send(msg);
   max_altitude=altitude;
}

else if ((altitude > 1300) && (FLIGHT_STATUS < 11) && (altitude>max_altitude))//define launch--playa is 1191m, road 1270m, mine peak 1328
 {
   FLIGHT_STATUS= 11;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[11])));
   tweet_send(msg);
   max_altitude=altitude;
}

else if ((altitude > 4412) && (FLIGHT_STATUS < 12) && (altitude>max_altitude)) //4412 Highest mountain in CA
{
   FLIGHT_STATUS= 12;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[12])));
   tweet_send(msg);
   max_altitude=altitude;
}


else if ((altitude > 8858) && (FLIGHT_STATUS < 13) && (altitude>max_altitude)) //8858 Mt. Everest
{
   FLIGHT_STATUS= 13;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[13])));
   tweet_send(msg);
   max_altitude= altitude;
}


else if ((altitude > 10600) && (FLIGHT_STATUS < 14) && (altitude>max_altitude)) //10600 Airliner
{
   FLIGHT_STATUS= 14;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[14])));
   tweet_send(msg);
   max_altitude= altitude;
}
else if ((altitude > 18000) && (FLIGHT_STATUS < 15) && (altitude>max_altitude)) //18000 Darkness of Space???
{
   FLIGHT_STATUS= 15;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[15])));
   tweet_send(msg);
   max_altitude= altitude;
}

else if ((altitude > 23096) && (FLIGHT_STATUS < 16) && (altitude>max_altitude)) //23096 IKaros 1
{
   FLIGHT_STATUS= 16;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[16])));
   tweet_send(msg);
   max_altitude= altitude;
}

else if ((altitude > 24000) && (FLIGHT_STATUS < 17) && (altitude>max_altitude)) //24000 SR71 Blackbird
{
   FLIGHT_STATUS= 17;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[17])));   
   tweet_send(msg);
   max_altitude= altitude;
}
else if ((altitude > 25534) && (FLIGHT_STATUS < 18) && (altitude>max_altitude)) //25534 Ikaros 2
 {
   FLIGHT_STATUS= 18;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[18])));
   tweet_send(msg);   max_altitude= altitude;
   max_altitude= altitude;
}
else if ((altitude > 30480) && (FLIGHT_STATUS < 19) && (altitude>max_altitude)) //30480 100,000 ft !!! :0
{
   FLIGHT_STATUS= 19;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[19])));
   tweet_send(msg);
   max_altitude= altitude;
}

else if ((altitude > 32004) && (FLIGHT_STATUS < 20) && (altitude>max_altitude)) //32004 105,000
{
   FLIGHT_STATUS= 20;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[20])));
   tweet_send(msg);
   max_altitude= altitude;
}

  else if ((altitude > 33528) && (FLIGHT_STATUS < 21) && (altitude>max_altitude)) //33528 110,000
{
   FLIGHT_STATUS= 21;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[21])));
   tweet_send(msg);
   max_altitude= altitude;
}
  else if ((altitude > 35022) && (FLIGHT_STATUS < 22) && (altitude>max_altitude)) //35022 115,000
{
   FLIGHT_STATUS= 22;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[22])));
   tweet_send(msg);
   max_altitude= altitude;
}
  else if ((altitude > 36576) && (FLIGHT_STATUS < 23) && (altitude>max_altitude)) //36576 120,000
{
   FLIGHT_STATUS= 23;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[23])));
   tweet_send(msg);
   max_altitude= altitude;
}

  else if ((altitude<(max_altitude-100)) && (FLIGHT_STATUS > 11) && (FLIGHT_STATUS < 31))  //Burst ****100m downdraft/gps error buffer
{
   FLIGHT_STATUS= 31;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[31])));
   tweet_send(msg);
   aprs_send(max_altitude);
   
   
//FALLING: Order reversed because high altitude would trigger even if not that high.
}
  else if ((altitude < 3048) && (max_altitude > 3048) && (FLIGHT_STATUS < 45) && (FLIGHT_STATUS >= 31)) //3048 Falling at 10,000 ft
 {
   FLIGHT_STATUS= 45;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[45])));
   tweet_send(msg);
}
  else if ((altitude < 7620) && (max_altitude > 7620) && (FLIGHT_STATUS < 44) && (FLIGHT_STATUS >= 31)) //7620 Falling at 25,000 ft
 {
   FLIGHT_STATUS= 44;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[44])));
   tweet_send(msg);
}
  else if ((altitude < 15240) && (max_altitude > 22869) && (FLIGHT_STATUS < 43) && (FLIGHT_STATUS >= 31)) //15240 Falling at 50,000 ft
 {
   FLIGHT_STATUS= 43;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[44])));
   tweet_send(msg);
}
  else if ((altitude < 22860) && (max_altitude > 22860) && (FLIGHT_STATUS < 42) && (FLIGHT_STATUS >= 31)) //22860 Falling at 75,000 ft
 {
   FLIGHT_STATUS= 42;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[42])));
   tweet_send(msg);}
  else if ((altitude < 30480) && (max_altitude > 30480) && (FLIGHT_STATUS < 41) && (FLIGHT_STATUS >= 31) && (altitude<max_altitude)) //30480 Falling at 100,000 ft
 {
   FLIGHT_STATUS= 41;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[41])));
   tweet_send(msg);
}
 else if ((altitude < 4000) && (FLIGHT_STATUS < 51) && (FLIGHT_STATUS > 44) && (altitude == last_altitude) && (last_altitude == slast_altitude)) //3 equal altitudes after burst and below ***4000 and 25K status means landing
 {
   FLIGHT_STATUS= 51;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[51])));
   tweet_send(msg);
}
 else if ((FLIGHT_STATUS == 51) && (altitude == last_altitude) && (last_altitude == slast_altitude)) //second and third landing report on next cycle, or slight delay for altitude to be steady.
 {
   FLIGHT_STATUS= 52;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[52])));
   tweet_send(msg);
 }
 else if ((FLIGHT_STATUS == 52) && (altitude == last_altitude) && (last_altitude == slast_altitude))
 {
   FLIGHT_STATUS= 53;
   strcpy_P(msg, (char*)pgm_read_word(&(TWEETS[53])));
   tweet_send(msg);
 }

  else if (altitude>max_altitude)
    {max_altitude=altitude;} 
    
slast_altitude=last_altitude;
last_altitude=altitude;
//return(altitude);

}



/*
String RTCread()
{
    DateTime now = RTC.now();
    String timestamp = "";
    timestamp += now.year();
    timestamp += '/';
    timestamp += now.month();
    timestamp += '/';
    timestamp += now.day(), DEC;
    timestamp += ' ';
    timestamp += now.hour(), DEC;
    timestamp += ':';
    if (now.minute() < 10) {timestamp += '0';}
    timestamp += now.minute(), DEC;
    timestamp += ':';
    if (now.second() < 10) {timestamp += '0';}    
    timestamp += now.second(), DEC;
    return timestamp;  
}

void datarecord()
{
/*
  String dataString = "";
  int tempext = sensors_ext_thermistor();
//  int tempint = sensors_internal_temp();
  int pressure = sensors_pressure();
  String RTCtime = RTCread();
  
  dataString += RTCtime;
  dataString += ", ";
//  dataString += String(tempint);
//  dataString += ", ";
  dataString += String(tempext);
  dataString += ", ";
  dataString += String(pressure);

  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

//dataFile.close();//test
  
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }

}
*/

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(GPS_BAUDRATE);
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif  
  modem_setup();
//  buzzer_setup();
  sensors_setup();
  gps_setup();

//Data logger setup
/*
Wire.begin();
RTC.begin();
pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {

    // don't do anything more:
    return;
  }
*/
  // Schedule the next transmission within APRS_DELAY ms
  next_tx_millis = millis() + APRS_DELAY;
  next_tweet_millis = millis() + APRS_DELAY + 5000; //start status check for tweet after first APRS tx
//  next_data_millis = millis() + APRS_DELAY + 1000; //start data after first APRS tx
//  last_data_millis = millis() + APRS_DELAY;

}

void loop()
{
  int d;
  
  if (millis() >= next_tx_millis) {
    // Show modem ISR stats from the previous transmission
#ifdef DEBUG_MODEM
    modem_debug();
#endif
//    next_tx_millis = millis() + APRS_PERIOD; // original code
      next_tx_millis += APRS_PERIOD;
    aprs_send(max_altitude);

  }
  if (Serial.available()) {
    d = Serial.read();
    if (gps_decode(d)) {
  if (millis() >= next_tweet_millis) {
    // Show modem ISR stats from the previous transmission
#ifdef DEBUG_MODEM
    modem_debug();
#endif
    next_tweet_millis = millis() + TWEET_PERIOD;
    
    check_status(int(gps_altitude));

  }
    }

  }else {
    power_save();
  }
 

  
/*
  if (millis() >= (next_data_millis)) {
    next_data_millis = millis()+1000;
    datarecord();
    //next_data_millis = millis() + DATA_PERIOD;
  }
*/
/* 
  int c;
  if (Serial.available()) {
    c = Serial.read();
    if (gps_decode(c)) {
      if (gps_altitude > BUZZER_ALTITUDE)
        buzzer_off();   // In space, no one can hear you buzz
      else
        buzzer_on();
    }
  } else {
    power_save();
  }
 */
}

