#include <IRremote2.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <SPI.h>

#define SAD 10
#define RST 9

int RECV_PIN = 13;
int LedOUT = 22;
long on1  = 0x00FF6897;//編碼示例，與發送匹配用
long off1 = 0x00ff30CF;
long error = 0x00FFFFFFFF; 
long tag1=0x00F91A1679; 

LiquidCrystal lcd(12,11,2,3,4,5);
IRrecv irrecv(RECV_PIN);

decode_results results;

MFRC522 nfc(SAD, RST);

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(11,OUTPUT);
  lcd.begin(20, 2);
  lcd.print("Arduino");
  SPI.begin();
  nfc.begin();
  // Get the firmware version of the RFID chip
  byte version = nfc.getFirmwareVersion();
  if (! version) {
    Serial.print("Didn't find MFRC522 board.");
    while(1); //halt
  }

  Serial.print("Found chip MFRC522 ");
  Serial.print("Firmware ver. 0x");
  Serial.print(version, HEX);
  Serial.println(".");
}

int HaveInFlag;
void loop() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  int i, j, pos;
  long str=0;
  
  status = nfc.requestTag(MF1_REQIDL, data);
  if (status == MI_OK) {
    Serial.println("Tag detected.");
    Serial.print("Type: ");
    Serial.print(data[0], HEX);
    Serial.print(", ");
    Serial.println(data[1], HEX);
      
    // calculate the anti-collision value for the currently detected
    // tag and write the serial into the data array.
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    Serial.println("The serial nb of the tag is:");
    for (i = 0; i < 3; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(", ");
       str =( str << 8 ) |serial[i];
    }
    Serial.println(serial[3], HEX);
    str =( str << 8 ) |serial[3];
    Serial.print("this is recive tag code: ");
    Serial.println( str ,HEX);
    lcd.setCursor(0,1);
    lcd.print("Recive RFID:");
    lcd.setCursor(12,1);
    lcd.print(str,HEX); 
    HaveInFlag = 1;
     
    if (tag1==str) 
    {
      digitalWrite(LedOUT,HIGH);
      lcd.setCursor(10,0);
      lcd.print("OPEN DOOR ");
    }
    nfc.haltTag();
  }

  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    lcd.setCursor(0,1);
    lcd.print("Recive IR:");
    if (results.value != error)
    {
      lcd.print("          ");
      lcd.setCursor(10,1);
      lcd.print(results.value,HEX);
    }
    if(results.value == on1)
    {
      digitalWrite(LedOUT,HIGH);
      Serial.println("REALY ON");
      lcd.setCursor(10,0);
      lcd.print("Relay ON ");
    }
    if(results.value == off1)
    {
      digitalWrite(LedOUT,LOW);  
      Serial.println("REALY OFF");
      lcd.setCursor(10,0);
      lcd.print("Relay OFF");
    }
    irrecv.resume(); // Receive the next value
  }
  if (HaveInFlag == 1)
  {
    delay(2000);
    digitalWrite(LedOUT,LOW);
    HaveInFlag = 0;
    lcd.setCursor(10,0);
    lcd.print("Close Door");
    lcd.setCursor(12,1);
    lcd.print("        ");    
  } 
}
