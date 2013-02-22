#include <SD.h>

File logger;
void setup()
{
  //opens for serial port for debuging
  Serial.begin(9600);
    while (!Serial) {
      ; //not need for uno left here for compatablitiy sake
    }
    Serial.print("Initializing SD card, make sure SD card works before flying");
    //Write pin for SD card
    pinMode(10, OUTPUT);
    if (!SD.begin(4)) {
      Serial.println("dont fly the SD doesnt work");
      return;
    }
    //The SD card works! good job you didn't bjork it
    Serial.println("The SD works; you don't have the mental capacity of a koala!");
    logger = SD.open("logs/log.txt", FILE_WRITE);
    logger.println("koala's have little mental capacity!");
    
}

void loop() {
}
