#include <Wire.h>
#define DS1307_CTRL_ID 0x68
#define  NBRREGS 0x20

void setup() {
unsigned char i;

 Wire.begin();
 Serial.begin(9600);
 Serial.println(""); 
   Wire.beginTransmission(DS1307_CTRL_ID);
   Wire.write((uint8_t)0x00); 
  if (Wire.endTransmission() == 0) {
  Wire.requestFrom(DS1307_CTRL_ID, NBRREGS);
  if (Wire.available() >= NBRREGS) {
    for (i=0;i<NBRREGS;i++){
      Serial.print (Wire.read());
      Serial.print(" ");
    } 
  } else Serial.println ("Error reading all registers");
  } else Serial.println ("Error Detecting ds1307");
  Serial.println("");
  Serial.println ("Writing ram registers");


  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write((uint8_t)0x08); // set ptr to ram
  for (i=0x08;i<NBRREGS;i++) Wire.write(i);
  if (Wire.endTransmission() != 0) {
    Serial.println ("No ds1307...");
  }
  Serial.println ("Reading ram registers");

   Wire.beginTransmission(DS1307_CTRL_ID);
   Wire.write((uint8_t)0x00); 
  if (Wire.endTransmission() == 0) {
    Wire.requestFrom(DS1307_CTRL_ID, NBRREGS);
  if (Wire.available() >= NBRREGS) {
    for (i=0;i<NBRREGS;i++){
      Serial.print (Wire.read());
      Serial.print (" ");
    } 
  } else Serial.println ("Error reading all registers");
}  
}
void loop () {
}
