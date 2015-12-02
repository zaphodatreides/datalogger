#include <Time.h>
#include <DS1307RTC.h>
#include <Wire.h>
#define SEPARATOR ";"

tmElements_t tm;
String dataString;

void setup(void)
{
  
  Serial.begin(9600);
  while (!Serial) {;}
  Serial.print("Serial port initialized...");
 }

 void loop (void)
 {
   
   if (RTC.read(tm)) {
dataString= String(tm.Day)+"-"+String(tm.Hour)+":"+String(tm.Minute);
} else {
  if (RTC.chipPresent()) { tm.Day=8;
  tm.Month=11;
  tm.Year=CalendarYrToTm(2015);
  tm.Hour=0;
  tm.Minute=0;
  tm.Second=0;
  RTC.write(tm);
  dataString= String(tm.Day)+"-"+String(tm.Hour)+":"+String(tm.Minute);      
  } else dataString=String(millis());
}
   Serial.print (dataString);
   Serial.print (SEPARATOR);
   Serial.print (analogRead(0));
   Serial.print (SEPARATOR);
   Serial.print (analogRead(1));
   Serial.print (SEPARATOR);
   Serial.print (analogRead(2));
   Serial.println ();

 }
