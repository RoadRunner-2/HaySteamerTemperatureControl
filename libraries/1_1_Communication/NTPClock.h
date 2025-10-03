#ifndef NTPClock_h
#define NTPClock_h

#include <TimeLib.h>

// get current time from NTP
IPAddress timeServer(162, 159, 200, 123); // pool.ntp.org NTP server
static const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
const int timeZone = 1;     // Central European Time
const unsigned int localPort = 2390; // local port to listen for UDP packets
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}

bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int getDayOfWeek(int year, int month, int day) {
  // Zeller's congruence algorithm
  if (month < 3) {
    month += 12;
    year--;
  }
  
  int k = year % 100;
  int j = year / 100;
  
  int h = (day + ((13 * (month + 1)) / 5) + k + (k / 4) + (j / 4) - 2 * j) % 7;
  
  // Convert to Sunday = 0, Monday = 1, etc.
  return (h + 5) % 7;
}

int getLastSundayOfMonth(int year, int month) {
  // Find the last day of the month
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  // Check for leap year
  if (month == 2 && isLeapYear(year)) {
    daysInMonth[1] = 29;
  }
  
  int lastDay = daysInMonth[month - 1];
  
  // Find the day of week for the last day of the month
  int dayOfWeek = getDayOfWeek(year, month, lastDay);
  
  // Calculate how many days to subtract to get to Sunday (0 = Sunday)
  int daysToSubtract = dayOfWeek;
  
  return lastDay - daysToSubtract;
}

unsigned long add_daylight_saving_time(const unsigned long& timeStamp)
{
  time_t t = timeStamp;
  struct tm* utc_tm = gmtime(&t);
  
  int year = utc_tm->tm_year + 1900;
  int month = utc_tm->tm_mon + 1;
  int day = utc_tm->tm_mday;
  int hour = utc_tm->tm_hour;
  int minute = utc_tm->tm_min;
  
  // German DST Rules (same as EU):
  // Starts: Last Sunday in March at 02:00 CET becomes 03:00 CEST
  // Ends: Last Sunday in October at 03:00 CEST becomes 02:00 CET
  
  // Before March or after October: no DST
  if (month < 3 || month > 10) {
    return timeStamp;
  }
  
  // April through September: always DST
  if (month > 3 && month < 10) {
    return timeStamp + SECS_PER_HOUR;
  }
  
  // March: check if we're past the last Sunday at 02:00 CET
  if (month == 3) {
    int lastSundayMarch = getLastSundayOfMonth(year, 3);
    if (day < lastSundayMarch) {
      return timeStamp;
    } else if (day > lastSundayMarch) {
      return timeStamp + SECS_PER_HOUR;
    } else {
      // On the transition day, check the hour
      return timeStamp + (int(hour >= 2)) * SECS_PER_HOUR; // DST starts at 02:00 CET
    }
  }
  
  // October: check if we're before the last Sunday at 02:00 CET
  if (month == 10) {
    int lastSundayOctober = getLastSundayOfMonth(year, 10);
    if (day < lastSundayOctober) {
      return timeStamp + SECS_PER_HOUR;
    } else if (day > lastSundayOctober) {
      return timeStamp;
    } else {
      // On the transition day, check the hour
      return timeStamp + (int(hour < 2)) * SECS_PER_HOUR; // DST ends at 02:00 CET
    }
  }
  
  return timeStamp;
}

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return add_daylight_saving_time(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void setup_ntp_clock()
{
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);

  setSyncProvider(getNtpTime);
  setSyncInterval(60*60); // update once an hour
}


#endif
