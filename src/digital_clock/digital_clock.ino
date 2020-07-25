#include <Wire.h>
#include <LiquidCrystal.h>

#define ds1307Addr 0b1101000

#define controlRegisterAddr 0x07
#define secondRegisterAddr 0x00
#define dayRegisterAddr 0x03

#define initialDayCount 0x06
#define initialDate 0x25
#define initialMonth 0x07
#define initialYear 0x20

// Hour in 12hr format
#define initialHour 0b01100110
#define initialMinute 0x38
#define initialSecond 0x00

// Set to true if you want to set initial time during programming
#define setTime false

LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 8);


//Get day name from day count
String getDayName(int dayCount) {
  switch (dayCount) {
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
  switch (monthCount) {
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

  Wire.beginTransmission(ds1307Addr);
  // Select control register
  Wire.write(controlRegisterAddr);
  // Set everything to 0 as no sqaure wave output needed
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1);

  if (setTime) {
    Wire.beginTransmission(ds1307Addr);
    Wire.write(dayRegisterAddr);
    Wire.write(initialDayCount);
    Wire.write(initialDate);
    Wire.write(initialMonth);
    Wire.write(initialYear);
    Wire.endTransmission();
    delay(1);

    Wire.beginTransmission(ds1307Addr);
    // Select second's register
    Wire.write(secondRegisterAddr);
    Wire.write(initialSecond);
    Wire.write(initialMinute);
    Wire.write(initialHour);
    Wire.endTransmission();
    delay(1);
  }
  lcd.begin(16, 2);
  lcd.setCursor(5, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Initializing....");
  delay(3000);
  lcd.clear();
}


int seconds, minutes, hours, receivedByteCount = 0, date, month, year, day;
String amOrPm;

void loop() {
  Wire.beginTransmission(ds1307Addr);
  Wire.write(secondRegisterAddr);
  Wire.endTransmission();
  Wire.requestFrom(ds1307Addr, 7);

  while (Wire.available()) {
    long int receivedByte = Wire.read();    // receive a byte as character
    switch (receivedByteCount) {
      case 0: seconds = receivedByte;
        break;
      case 1: minutes = receivedByte;
        break;
      case 2: hours = receivedByte;
        break;
      case 3: day = receivedByte;
        break;
      case 4: date = receivedByte;
        break;
      case 5: month = receivedByte;
        break;
      case 6: year = receivedByte;
        break;
    }
    ++receivedByteCount;
  }

  if (hours & 0b00100000) {
    amOrPm = "PM";
  } else {
    amOrPm = "AM";
  }

  hours &= 0b00011111;

  lcd.setCursor(0, 0);

  if (hours < 0x10) {
    lcd.print("0");
  }

  lcd.print(hours, HEX);
  lcd.print(":");

  if (minutes < 0x10) {
    lcd.print("0");
  }

  lcd.print(minutes, HEX);
  lcd.print(":");

  if (seconds < 0x10) {
    lcd.print("0");
  }

  lcd.print(seconds, HEX);
  lcd.print(" ");
  lcd.print(amOrPm);
  lcd.setCursor(0, 1);

  if (date < 0x10) {
    lcd.print("0");
  }

  lcd.print(date, HEX);
  lcd.print("-");
  lcd.print(getMonthName(month));
  lcd.print("-20");
  lcd.print(year, HEX);
  lcd.print(" ");
  lcd.print(getDayName(day));

  receivedByteCount = 0;

  delay(1000);
}
