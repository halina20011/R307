/*#################################################################################
 * This project was produced by Halina
 * 
 *  github: https://github.com/halina20011/R307
 *  YouTube: https://www.youtube.com/watch?v=QE2UWFv8szw&t
 *  
 *  Install Libraries From: Adafruit_Fingerprint.h      - https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
 * 
###################################################################################*/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup(){
  Serial.begin(9600);
  while (!Serial);  
  delay(100);
  Serial.println("\n\nDelete Finger");

  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
}

unsigned int readnumber(void) {
  unsigned int num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  while(Serial.available() > 0){
    String serial = Serial.readString();
    int number = serial.toInt();
    Serial.println(serial);
    Serial.println(number);
    if(number > 0){
      Serial.println("Numdber");
      for(int i = 1; i <= number; i++){
        Serial.println(i);
        deleteFingerprint(i);
      }
      templateCount();
    }
  }
}


unsigned int deleteFingerprint(unsigned int id) {
  unsigned int p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

int templateCount(){
  int a = 0;
  while(a == 0){
    finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    return 0;
  }
}
