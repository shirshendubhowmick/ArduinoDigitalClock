#include <Wire.h>
#include <LiquidCrystal.h>

#define ds1307Addr 0x68

#define controlRegisterAddr 0x07
#define secondRegisterAddr 0x00
#define dayRegisterAddr 0x03

#define initialDayCount 0x06
#define initialDate 0x25
#define initialMonth 0x07
#define initialYear 0x20

// Hour in 12hr format
#define initialHour 0b01000010
#define initialMinute 0x38
#define initiaSecond 0x00

// Set to true if you want to set initial time during programming
#define setTime false

LiquidCrystal lcd(2,3,4,5,6,7,8);


//Get day name from day count
String getDayName(int dayCount) {
  switch(dayCount) {
    case 1: return "MON";
    case 2: return "TUE";
    case 3: return "WED";
    case 4: return "THU";
    case 5: return "FRI";
    case 6: return "SAT";
    case 7: return "SUN";
  }
}

// Get month name from month count
String getMonthName(int monthCount) {
  switch(monthCount) {
    case 1: return "JAN";
    case 2: return "FEB";
    case 3: return "MAR";
    case 4: return "APR";
    case 5: return "MAY";
    case 6: return "JUN";
    case 7: return "JUL";
    case 8: return "AUG";
    case 9: return "SEP";
    case 0x10: return "OCT";
    case 0x11: return "NOV";
    case 0x12: return "DEC";
  }
}


void setup() {
  // Join I2C bus as master
  Wire.begin();

  if(setTime) {
    Wire.beginTransmission(ds1307Addr);
    // Select control register
    Wire.write(controlRegisterAddr);
    // Set everything to 0 as no sqaure wave output needed
    Wire.write(0x00);
    Wire.endTransmission();
    delay(1);
    Wire.beginTransmission(ds1307Addr);
    // Select second's register
    Wire.write(secondRegisterAddr);
    Wire.write(initiaSecond);
    Wire.write(initialMinute);
    Wire.write(initialHour);
    Wire.endTransmission();
    delay(1);
    Wire.beginTransmission(ds1307Addr);
    Wire.write(dayRegisterAddr);
    Wire.write(initialDayCount);
    Wire.write(initialDate);
    Wire.write(initialMonth);
    Wire.write(initialYear);
    Wire.endTransmission();
    delay(1);
  }
  lcd.begin(16,2);
  lcd.setCursor(5,0);
  lcd.print("Welcome");
  lcd.setCursor(0,1);
  lcd.print("Initializing....");
  delay(5000);
  lcd.clear();
}


int ss, mm, hh, cnt=0, DD, MM, YY, day;
String amOrPM;
void loop() {
  Wire.beginTransmission(ds1307Addr);
  Wire.write(secondRegisterAddr);
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 7);
 
  while(Wire.available()) { 
    long int c = Wire.read();    // receive a byte as character
    switch (cnt) {
      case 0: ss=c;
              break;
      case 1: mm=c;
              break;
      case 2: hh=c;
              break;
      case 3: day=c;
              break;
      case 4: DD=c;
              break;
      case 5: MM=c;
              break;
      case 6: YY=c;
              break;
    }
    ++cnt;
  }
 
  if((hh&0b00100000)==0x00) {
    amOrPM="AM";
  } else {
    amOrPM="PM";
  }
 
  hh&=0b00011111;
  lcd.setCursor(0,0);
  if (hh<0x10) {
    lcd.print("0");
  }
 
  lcd.print(hh, HEX);
  lcd.print(":");
  if (mm<0x10) {
    lcd.print("0");
  }
  lcd.print(mm, HEX);
  lcd.print(":");
  if (ss<0x10) {
    lcd.print("0");
  }
  lcd.print(ss, HEX);
  lcd.print(" ");
  lcd.print(amOrPM);
  lcd.setCursor(0, 1);
  if (DD<0x10) {
    lcd.print("0");
  }
  lcd.print(DD, HEX);
  lcd.print("-");
  lcd.print(getMonthName(MM));
  lcd.print("-20");
  lcd.print(YY, HEX);
  lcd.print(" ");
  lcd.print(getDayName(day));
  cnt=0;
  delay(1000);
}
