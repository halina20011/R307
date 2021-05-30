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
int buzzer = 12;
int ledGreen = 7;
int ledRed = 6;
int good = 0;

void setup(){
  Serial.begin(9600);
  noTone(buzzer);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  while (!Serial);  
  delay(100);
  Serial.println("\n\nR307 finger detect test");
  finger.begin(57600);   // set the data rate for the sensor serial port
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
}

void loop(){
  getFingerprintIDez();
  if(good == 1){
    correctFingerprint(ledGreen, buzzer);
    Serial.print("Remove Your Finger From Detector!!!");
    good = false;
  }
  else if (good == 2){
    incorrectFingerprint(ledRed, buzzer);
  } 
  good = 0;         
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK){
    //Serial.println("Not Found - Taze");
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Not Found - Taze 1");
    good = 0;
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("Not Found - Taze 2");
    good = 2;
    return -1;
  }
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  good = 1;
  return finger.fingerID; 
}


int correctFingerprint(int led, int Buzzer){
  digitalWrite(led, HIGH);
  tone(Buzzer, 500);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  noTone(Buzzer);
  delay(500);
  digitalWrite(led, LOW);
  return 1;
}

int incorrectFingerprint(int led, int Buzzer){
  Serial.println("Wrong");
  digitalWrite(led, HIGH);
  tone(buzzer, 300);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  noTone(buzzer);
  delay(500);
  digitalWrite(led, LOW);
  return 2;
}
