

#include <SD.h>

const int chipSelect = 10;
int tempPin = A0;
int time = 0;
int tempValue = 0;
int presValue = 0;
int altiValue = 0;
int compValue = 0;
int timeValue = 0;

void setup()
{
  Serial.begin(9600);
   while (!Serial) {
    ; 
  }

  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
}

void datawrite() {
  
  String dataString = "";

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }  
  else {
    Serial.println("error opening datalog.txt");
  } 
}
void loop()
{
  void dataWrite();
}
