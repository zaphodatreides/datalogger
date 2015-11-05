/*
 * Based on the sketch for testing sleep mode with wake up on timer by Donal Morrissey - 2011.
 *
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <SdFat.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>


const int chipSelect = 10;

volatile int f_wdt=0;

SdFat SD;

#define LED 2
#define THERMO 3


/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect)
{  
  delay (500);
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


void setup()
{
  
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  pinMode (LED,OUTPUT);
  pinMode (THERMO,OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
   for (;;){
   digitalWrite(LED,HIGH);
   delay(1000);
   digitalWrite(LED,LOW);
   delay(1000);
	
  }
    return;
  }
  
  Serial.println("card initialized.");
  digitalWrite(LED,LOW);
  delay (500);
  digitalWrite(LED,HIGH);
  delay (250);
  digitalWrite(LED,LOW);
  delay (500);
  digitalWrite(LED,HIGH);
  delay(250);
  digitalWrite(LED,LOW);

  digitalWrite(chipSelect,HIGH);

  
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  

 }



/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  tmElements_t tm;
  String dataString = "";
  int i,j,k,l;
  unsigned int temp_table[61] PROGMEM ={843,834,825,815,806,796,786,776,765,754,743,732,721,709,700,686,674,662,649,637,625,612,600,587,575,562,550,537,525,512,500,488,476,463,452,440,428,417,405,394,383,373,362,352,342,332,322,312,303,294,285,276,268,260,252,244,236,229,222,215,208};
  unsigned int temp,lower,higher,fraction;
  unsigned int temp2,lower2,higher2,fraction2;    
  int pos=0;
  char * conv PROGMEM ={"0123456789"};



  // read three sensors and append to the string:
  j=0;
  k=0;
  l=0;
  f_wdt=1;  
  
  for (i=0;i<30;i++) {
  	digitalWrite (THERMO,HIGH);
   	delay(1);
        int sensor = analogRead(1);
        j+=sensor;
        sensor = analogRead(2);
        k+=sensor;
        sensor = analogRead(3);
        l+=sensor;
        digitalWrite (THERMO,LOW);
        if(f_wdt==1)
          {
            f_wdt = 0;
            enterSleep(); 
            //delay (200);
          }               
  }
k/=30;
j/=30;
l/=30;
if (RTC.read(tm)) {
dataString= String(tm.Day)+"-"+String(tm.Hour)+":"+String(tm.Minute);
} else dataString=String(millis());
dataString+=";";
  for (temp=0;temp<=60;temp++) if (temp_table[temp]<=j) break;
  if ((temp>0) && (temp<=60) && (j!=temp_table[temp])) {
    higher=temp_table[temp]; 
    lower=temp_table[temp-1];
    fraction=trunc(((lower-j)*10)/(lower-higher)); //9-
  } else fraction=0;
  
  for (temp2=0;temp2<=60;temp2++) if (temp_table[temp2]<=k) break;
  if ((temp2>0) && (temp2<=60) && (k!=temp_table[temp2])) {
    higher2=temp_table[temp2]; 
    lower2=temp_table[temp2-1];
    fraction2=trunc(((lower2-k)*10)/(lower2-higher2)); //9-
  } else fraction2=0;
  
 if (temp<20) { dataString+='-';
 dataString+=conv[(20-temp)/10];dataString+=conv[(20-temp)%10];} else{  
 dataString+=conv[(temp-20)/10];dataString+=conv[(temp-20)%10];}
 dataString+='.';
 dataString+=conv[fraction];
 dataString+=';';
 //String tempF=String(buffer);
 
 if (temp2<20) { dataString+='-';
 dataString+=conv[(20-temp2)/10];dataString+=conv[(20-temp2)%10];} else{  
 dataString+=conv[(temp2-20)/10];dataString+=conv[(temp2-20)%10];}
 dataString+='.';
 dataString+=conv[fraction2];

/*dataString+=String(f);
dataString+=",";
dataString+=String(k);
*/

dataString+=";";
dataString+=String(l);

 digitalWrite (chipSelect,LOW); 
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
  digitalWrite(chipSelect,HIGH);


}
