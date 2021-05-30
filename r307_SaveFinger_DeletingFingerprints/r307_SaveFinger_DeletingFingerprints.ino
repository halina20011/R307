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

int led = 13;
bool good = false;
String command = "";

unsigned int id;

void setup()  {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  while (!Serial);  
  delay(100);
  Serial.println("\n\nR307 finger detect test");
  
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("");
  Serial.println("All commands:  Command        Function ");
  Serial.println("               Save           Save new finger print.");
  Serial.println("               Delete         Delete finger print.");
  Serial.println("               TemplateCount  Writing a total of prints in memory.");
  Serial.println("               Check          Check fingerprint.");
  Serial.println("");
  Serial.println("Waiting for valid command...");
}

unsigned int readnumber(void) {
  unsigned int num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop(){
  if (Serial.available() > 0) {
    command = Serial.readString(); 

    Serial.print("Received: ");
    Serial.println(command);
    if (command == "Save" or command == "save" or command == "s"){ 
      Serial.println("Save...");
      saveFinger();
    }
    else if(command == "Delete" or command == "delete" or command == "d"){
      removeFinger();
    }
    else if(command == "TemplateCount" or command == "templateCount" or command == "t"){
      templateCount(); 
    }
    else if(command == "Check" or command == "Check" or command == "c"){
      checkFinger(); 
    }
    else {
      Serial.println("Wrong command");
    }
  }
}

unsigned int getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(1);
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
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
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

int getFingerprintIDez() {
  unsigned int p = finger.getImage();
  if (p != FINGERPRINT_OK){
    Serial.println("Not Found 1");
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Not Found 2");
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("Not Found 3");
    return -1;
  }
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID); // found a match!
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  good = true;
  return finger.fingerID; 
}

unsigned int getFingerprintID() {
  unsigned int p = finger.getImage();
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
  
  int id = finger.fingerID;
  int confidence = finger.confidence;
  String string1 = "Found ID #" + id;
  String string2 = " with confidence of " + confidence;
  String string = string1 + string2;
  Serial.println(string); 

  return finger.fingerID;
}

int saveFinger(){
  int a = 0;
  while(a == 0){
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 1000) you want to save this finger as...");
    id = readnumber();
    if (id == 0) {
       return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    
    while (!  getFingerprintEnroll() );
    return 0;
  }
}

int removeFinger(){
  int a = 0;
  while(a == 0){
    Serial.println("Please type in the ID # (from 1 to 1000) you want to delete...");
    unsigned int id = readnumber();
    if (id == 0) {
      return;
    }

    Serial.print("Deleting ID #");
    Serial.println(id);
  
    deleteFingerprint(id);
    return 0;
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

int checkFinger(){
  int a = 0;
  while(a == 0){
    getFingerprintIDez();
    if(good == true){
      Serial.println("Remove Your Finger From Detector!!!");
      blinkLed(led, 250, 2);
      good = false;
    }
    //return 0;
  }
}

int blinkLed(int Led, float Delay, int times){
  if(times == 1){
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    return 1;
  }
  if(times == 2) {
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    digitalWrite(Led, HIGH);
    delay(Delay);
    digitalWrite(Led, LOW);
    delay(Delay);
    return 2;
  }
}
