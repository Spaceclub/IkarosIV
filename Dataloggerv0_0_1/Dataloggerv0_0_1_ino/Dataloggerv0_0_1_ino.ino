#include <SD.h>
const int chipSelect = 10;
void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card"); //Delete for real thing
  pinMode(10, OUTPUT); 
  if (!SD.begin(chipSelect)) {
  Serial.println("card failed"); //Delete for real thing
  return;
  } 
  Serial.println("card initialized"); //Delete for real thing
}
void datalog() 
{
  String dataStrint = "";
  File dataFile = SD.open("log.txt", FILE_WRITE);
  if (dataFile) {
  dataFile.println(dataString);
  dataFile.close();
  }
  else {
  Serial.println("bjork"); //Delete for real thing
  }
}
