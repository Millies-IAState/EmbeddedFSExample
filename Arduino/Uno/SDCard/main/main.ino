/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

const uint8_t chipSelect = SS;
File myFile;
int32_t FAT32MetaSize = 32; //bytes per Directory Entry
int32_t randomFileCount = 110; //Number of files to create and read.
int32_t randomDataSize = randomFileCount * FAT32MetaSize; //#file time directory entry size
const int32_t seqDataCount = 3200; //Characters to write.
int32_t seqDataSize = seqDataCount + FAT32MetaSize; //file characters + 1 file directory entry

unsigned long rwTest = 0; //Random Write Test Time
unsigned long rrTest = 0; //Random Read Test Time
unsigned long swTest = 0; //Random Read Test Time
unsigned long srTest = 0; //Random Read Test Time
char seqRData[seqDataCount] = {'a'};
const PROGMEM char seqWData[seqDataCount] = {'a'};


void ClearTestFiles(int fileCount)
{
  //Remove any Existing files
  for(int i = 0; i < fileCount; i++)
  {
    SD.remove(i+".txt");
  }
}

unsigned long RandomWriteTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  ClearTestFiles(randomFileCount); //Clear the Test Space.

  start = micros();
  for(int i = 0; i < randomFileCount; i++)
  {
    myFile = SD.open(String(i)+".txt", FILE_WRITE);
    myFile.close();
  }
  end = micros();
  return end - start;
}

unsigned long RandomReadTest()
{
  unsigned long start =0;
  unsigned long end = 0;

  start = micros();
  for(int i = 0; i < randomFileCount; i++)
  {
    myFile = SD.open(String(i)+".txt", FILE_READ);
    myFile.close();
  }
  end = micros();
  return end - start;
}

unsigned long SquentialWriteTest()
{
  unsigned long start =0;
  unsigned long end = 0;

  start = micros();
  myFile = SD.open("sequential.txt", FILE_WRITE);
  myFile.write(seqWData,seqDataCount);
  myFile.close();
  end = micros();
    
  return end - start;
}

unsigned long SquentialReadTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  
  start = micros();
  myFile = SD.open("sequential.txt", FILE_READ);
  myFile.read(seqRData,seqDataCount);
  myFile.close();
  end = micros();
  return end - start;
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
     // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // Serial.println("Starting Random Write Test.");
  // rwTest = RandomWriteTest(randomFileCount);
  // Serial.write("Random Write Time (microseconds),");
  // Serial.println(rwTest);

  // Serial.println("Starting Random Read Test.");
  // rrTest = RandomReadTest(randomFileCount);
  // Serial.write("Random Read Time (microseconds),");
  // Serial.println(rrTest);

  // Serial.println("Starting Sequential Write Test.");
  // swTest = SquentialWriteTest();
  // Serial.write("Sequential Write Time (microseconds),");
  // Serial.println(swTest);

  // Serial.println("Starting Sequential Read Test.");
  // srTest = SquentialReadTest();
  // Serial.write("Sequential Read Time (microseconds),");
  // Serial.println(srTest);
  Serial.println("Run_Num,Random_Write,Random_Read,Seq_Write,Seq_Read");
}


int i = 0;
void loop() {
  
  rwTest = RandomWriteTest();
  rrTest = RandomReadTest();
  swTest = SquentialWriteTest();
  srTest = SquentialReadTest();

  Serial.print(i);
  Serial.write(",");
  Serial.print(rwTest);
  Serial.write(",");
  Serial.print(rrTest);
  Serial.write(",");
  Serial.print(swTest);
  Serial.write(",");
  Serial.println(srTest);
  i++;
  if(i>=100)
  {
    while(1){};
  }
}


