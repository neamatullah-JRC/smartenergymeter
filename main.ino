#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

const int VT_PIN = A0;
const int VT_RATIO = 1000;
const int CT_PIN = A1;


int out = 8;
SoftwareSerial gsm(2, 3); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

float energy, power_offset;
unsigned long time_prev, time_now;
  String mystring;

String incomingData = ""; 

void setup() {
  lcd.init();
  lcd.backlight();
    
  Serial.begin(9600);
  gsm.begin(9600);
  power_offset = 0;
  energy = 0;
  time_prev = millis();
  incomingData.reserve(200);

      showInitializingMsg();
       gsm.println("AT+CMGF=1");
  delay(200);

  // set gsm module to receive sms & show the output on serial
  gsm.println("AT+CNMI=2,2,0,0,0");
  delay(200);

   //   sendMessage("system ready");

}

void loop() {

  receiveSMS();
  int vtReading = analogRead(VT_PIN);
  int voltage = (vtReading * 5.0) / 1023.0 * VT_RATIO;
  float currentReading = analogRead(CT_PIN);;
  float current = (currentReading * 5.0) / 1023.0 * 1000;
        current = current/900;
  int power = voltage * current;
  time_now = millis();
  energy += ((time_now - time_prev) / 1000.0) * (power - power_offset)/1000/2;
  time_prev = time_now;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage);
  lcd.print("V");
  lcd.setCursor(7, 0);
  lcd.print("I: ");
  lcd.print(current);
  lcd.print(" A");
  lcd.setCursor(0, 1);
  lcd.print("W:");
  lcd.print(power);
  lcd.print(" ");
   lcd.setCursor(6, 1);
  lcd.print("U:");
  lcd.print(energy);
  lcd.print(" KWH");
  delay(1000);
mystring = String(energy);
float bill = energy*7.00;

 if (incomingData.indexOf("bill") >= 0) {

   sendMessage(mystring,bill);
  Serial.println("bill");
}

    incomingData = "";
}


void showInitializingMsg() {
  
  String msg = "Initializing";
  unsigned int restOfCellInLine = (16 - msg.length());
  lcd.clear();
  lcd.print(msg);

  for (int i = 0; i < restOfCellInLine; i++) {
    for (int j = 0; j < restOfCellInLine; j++) {
      lcd.print(".");
      delay(1000);
    }
    lcd.clear();
    lcd.print(msg);
  }
}
void receiveSMS() {
  while (gsm.available()) {
    incomingData = gsm.readStringUntil('\0'); // Get the data from the serial port.
    incomingData.toLowerCase();
    Serial.print(incomingData);
  }
}


void sendMessage(String msg, float bill) {
  gsm.println("AT+CMGF=1");    // Set the GSM Module in Text Mode
  delay(1000);
  gsm.println("AT+CMGS=\"+8801920639338\""); // Replace it with your mobile number
  
  delay(1000);
  
  gsm.println("Total UNIT:");
  gsm.print(msg);
  gsm.print(" KWH");
  gsm.print("          " );
  gsm.print("Unit price: 7.00");
  gsm.print("          " );
  gsm.print("Total bill:");
  gsm.print(bill);
  gsm.print("TK"); 
  // The SMS text you want to send
  delay(1000);
  gsm.println((char)26);  // ASCII code of CTRL+Z
  delay(1000);
  gsm.println();
  delay(1000);
}
