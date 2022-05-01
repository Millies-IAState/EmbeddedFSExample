/* mbed Microcontroller Library
 * Copyright (c) 2006-2019 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <functional>
#include <string>

#include "BlockDevice.h"
#include "HeapBlockDevice.h"

// Maximum number of elements in buffer
#define BUFFER_MAX_LEN 10
#define FORCE_REFORMAT true

using namespace std::chrono;

BlockDevice *BlockDevice::get_default_instance()
{
    static HeapBlockDevice hbd(8192,256);
    return &hbd;
}
// This will take the system's default block device
BlockDevice *bd = BlockDevice::get_default_instance();
FILE *f;

// Instead of the default block device, you can define your own block device.
// For example: HeapBlockDevice with size of 2048 bytes, read size 1, write size 1 and erase size 512.
// #include "HeapBlockDevice.h"
// BlockDevice *bd = new HeapBlockDevice(2048, 1, 1, 512);


// This example uses LittleFileSystem as the default file system
#include "LittleFileSystem.h"
LittleFileSystem fs("fs");

// Uncomment the following two lines and comment the previous two to use FAT file system.
// #include "FATFileSystem.h"
// FATFileSystem fs("fs");


// Set up the button to trigger an erase
InterruptIn irq(BUTTON1);
void erase() {
    printf("Initializing the block device... ");
    fflush(stdout);
    int err = bd->init();
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }

    printf("Erasing the block device... ");
    fflush(stdout);
    err = bd->erase(0, bd->size());
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }

    printf("Deinitializing the block device... ");
    fflush(stdout);
    err = bd->deinit();
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
}

static auto erase_event = mbed_event_queue()->make_user_allocated_event(erase);

long micros()
{
    auto now_ms = time_point_cast<microseconds>(Kernel::Clock::now()); // Convert time_point to one in microsecond accuracy
    long micros = now_ms.time_since_epoch().count();
    return micros;
}
static int32_t randomFileCount = 110; //Number of files to create and read.
static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static int setSize = 62;
static char buff[11] = "aaaaaaaaaa";

static void RandomizeString()
{
    int key;
    for(int i = 0; i<10;i++)
    {
        key = rand() % setSize;
        buff[i] = charset[key];
    }
}

unsigned long RandomWriteTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  int err = 0;
  char name[100] = "/fs/aaaaaaaaaa.txt";
  string val;

  start = micros();
  
  for(int i = 0; i < randomFileCount; i++)
  {
    val = to_string(i);
    memcpy(&name[4],&val,strlen(&val[0]));
    f = fopen(name, "w+");
    err = fclose(f);
  }
  end = micros();
  return end - start;
}

unsigned long RandomReadTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  int err = 0;
  char name[100] = "/fs/aaaaaaaaaa.txt";
  string val;

  start = micros();
  
  for(int i = 0; i < randomFileCount; i++)
  {
    val = to_string(i);
    memcpy(&name[4],&val,strlen(&val[0]));
    f = fopen(name, "r");
    err = fclose(f);
  }
  end = micros();
  return end - start;
}

char seqWData[1001] = "a";

unsigned long SquentialWriteTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  int err = 0;
  

  start = micros();
  
  for(int i = 0; i < randomFileCount; i++)
  {
    f = fopen("/fs/testData.txt", "w+");
    fprintf(f,seqWData);
    err = fclose(f);
  }
  end = micros();
  return end - start;
}

char seqRData[1001] = "b";

unsigned long SquentialReadTest()
{
  unsigned long start =0;
  unsigned long end = 0;
  int err = 0;

  start = micros();
  
  for(int i = 0; i < randomFileCount; i++)
  {
    f = fopen("/fs/testData.txt", "r");
    fgets(seqRData,1001,f);
    err = fclose(f);
  }
  end = micros();
  return end - start;
}

// Entry point for the example
int main() {
    printf("--- Mbed OS filesystem example ---\n");

    // Setup the erase event on button press, use the event queue
    // to avoid running in interrupt context
    irq.fall(std::ref(erase_event));

    // Try to mount the filesystem
    printf("Mounting the filesystem... ");
    fflush(stdout);
    int err = fs.mount(bd);
    printf("%s\n", (err ? "Fail :(" : "OK"));
    
    //Run the tests
    for(int i = 0; i<100; i++)
    {
        err = fs.reformat(bd);
        if (err) {
            error("error: %s (%d)\n", strerror(-err), err);
        }

        fflush(stdout);
        printf("%d,%ld,%ld,",i,RandomWriteTest(),RandomReadTest());


        err = fs.reformat(bd);
        if (err) {
            error("error: %s (%d)\n", strerror(-err), err);
        }

        seqWData[1000] = '\0';
        seqRData[1000] = '\0';
        for(int i = 0;i<1000;i++)
        {
            seqWData[i] = 'a';
        }
        
        fflush(stdout);
        printf("%ld,%ld\r\n",SquentialWriteTest(),SquentialReadTest());
    }

    // Tidy up
    printf("Unmounting... ");
    fflush(stdout);
    err = fs.unmount();
    printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
        
    printf("done!\n");
}

