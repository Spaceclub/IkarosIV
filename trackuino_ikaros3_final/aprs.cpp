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

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <WString.h>


// Module functions
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

// Exported functions
void aprs_send(int max_altitude)
{
  
  char temp[12];                   // Temperature (int/ext) //This comment doesn't seem correct--string is used for everything BUT temperature.
  const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };
 
 
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data. See APRS101p23
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
  ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                // Symbol table
  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte('O');                // Symbol: O=balloon, -=home v=van K=school
  snprintf(temp, 4, "%03d", (int)(gps_course + 0.5)); 
  ax25_send_string(temp);             // Course (degrees)
  ax25_send_byte('/');                // and
  snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
  ax25_send_string(temp);             // speed (knots)
  ax25_send_string("/A=");            // Altitude (feet). Goes anywhere in the comment area
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
  ax25_send_string(temp);
  ax25_send_string("/Max=");            // Max altitude so far
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(max_altitude) + 0.5));
  ax25_send_string(temp);
  ax25_send_string("/Ti=");
  ax25_send_string(itoa(sensors_int_lm60(), temp, 10));
  ax25_send_string("/Te=");
  ax25_send_string(itoa(sensors_ext_thermistor(), temp, 10));
  ax25_send_string("/hPa=");
  ax25_send_string(itoa(sensors_pressure(), temp, 10));
  ax25_send_byte(' ');
  ax25_send_string(APRS_COMMENT);     // Comment

  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
}

void tweet_send(const char *TWEET_MESSAGE)
{
  char temp[12];                   // Temperature (int/ext) 
  const struct s_address addresses[] = { 
    {D2_CALLSIGN, D2_CALLSIGN_ID},  // Destination callsign
    {S2_CALLSIGN, S2_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };
 
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
//  ax25_send_string(":KJ6DYP-7 :");
  ax25_send_string(":73S      :");
  ax25_send_string(TWEET_MESSAGE);
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
}
