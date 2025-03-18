/*
      step 1: mount SPIFFS      -   done
      step 2: connect to wifi   -   done
      step 3: download file     -   done 
*/
// #include <WiFi.h>
#include <WiFiManager.h>
#include <FS.h>
#include <SPIFFS.h>
#include <HTTPClient.h>

#define BUFFER_SIZE  2048      // adjust as needed

WiFiManager wifimanager;

String filename;

bool isNum(String input);
void PrintCommands();
void ListFiles();
void WriteFile();
void ReadFile();
void AppendFile();
void DeleteFile();
void FormatSPIFFS();
void DownloadFile();

void setup() 
{
  Serial.begin(115200);

  // for dynamic congiuring of wifi ssid and password 
  wifimanager.autoConnect("ESP32_WiFi_Config");

  // // for hard coding the wifi ssid and password 
  // WiFi.mode(WIFI_STA);
  // WiFi.begin("LENOVO laptop", "08!0g5Y7");              // start wifi connecion
  // Serial.println("Connecting...");

  // while(WiFi.status() != WL_CONNECTED)                   // wait till the wifi is connected
  // {
  //   Serial.print(".");
  //   delay(500);    
  // }
  
  if(SPIFFS.begin(true))                                    // Mounting SPIFFS 
    Serial.println("SPIFFS Mounted successfully");
  else
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  PrintCommands();  
}

void loop() 
{
  if(Serial.available())                                  // Perform operation only when user enters choice
  {
    String input = Serial.readStringUntil('\n');           // reads user input from serial monitor
    input.trim();                                          // remove white spaces

    if(isNum(input))
    {
      uint8_t choice = input.toInt();                      // converting string to int for switch case 
      
      Serial.println("Enter filename");
  
      while(!Serial.available());                          // wait for the user to enter the content
      filename = Serial.readStringUntil('\n');       // read the user input from serial monitor 
      filename.trim();   

      filename = "/" + filename;                          // adding '/' to filename to give path and store all files in root folder  
  
      switch(choice)
      {
        case 1:
          ListFiles();
        break;
        
        case 2:
          WriteFile();
        break;
        
        case 3:
          ReadFile();
        break;
        
        case 4:
          AppendFile();
        break;
        
        case 5:
          DeleteFile();
        break;
        
        case 6:
          FormatSPIFFS();
        break;
  
        case 7:
          DownloadFile();
        break;
  
        default:
          Serial.println("Invalid choice");
          PrintCommands();
        break;
      }
    }
  }    
}

// check if string is a number or not 
bool isNum(String input)
{
  for(uint8_t i = 0; i < input.length(); i++)
  {
    if(!isdigit(input.charAt(i)))
      return false;
  }
   
  return true;
}

void PrintCommands()
{
  unsigned long totalSpace = SPIFFS.totalBytes();
  unsigned long usedSpace = SPIFFS.usedBytes();
  unsigned long freeSpace = totalSpace - usedSpace;
  
  Serial.print("\n\n*************************************************************************\n\n"); 
  Serial.print("Total space (Bytes) : ");
  Serial.println(totalSpace);
  Serial.print("Used space (Bytes) : ");
  Serial.println(usedSpace);
  Serial.print("Free space (Bytes) : ");
  Serial.println(freeSpace);

  
  Serial.println("\nCommands (Enter the number of the command to be executed)");

  Serial.println("1. List all files");
  Serial.println("2. Write");
  Serial.println("3. Read");
  Serial.println("4. Append");
  Serial.println("5. Delete");
  Serial.println("6. Format SPIFFS");
  Serial.println("7. Download file");
  
}

// listing all the availble files in root folder
void ListFiles()
{
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("FILE : ");
    Serial.print(file.name());
    Serial.print("\t\t\tSIZE: ");
    Serial.println(file.size());

    file = root.openNextFile();
  }

  Serial.println("\n End");
  PrintCommands();
}


void WriteFile()
{
  File file = SPIFFS.open(filename, "w");              // open file in wrie mode 
  
  if(!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }

  Serial.println("Enter contents:");
  
  while(!Serial.available());                          // wait for the user to enter the content
  String content = Serial.readStringUntil('\n');       // read the user input from serial monitor 
  // content.trim();                                      // remove any leading or trailing white spaces

  if(content.length() > (SPIFFS.totalBytes() - SPIFFS.usedBytes()))    // for storge space available
  {
    Serial.println("Not enough space to write file");
    PrintCommands();
    return;
  }
    
  if(file.print(content))                                    // write to file 
    Serial.println("File written successfully");
  else 
    Serial.println("Failed to write file");
  
  file.close();
  PrintCommands();
}


void ReadFile()
{
  File file = SPIFFS.open(filename, "r");              // open file in read mode 
  
  Serial.println("File contents: \n");
  while(file.available())
  {
    Serial.write(file.read());                            // read filr contents and print on serial monitor
  }

  Serial.println("\nEnd of file");
  
  file.close();
  PrintCommands();
}


void AppendFile()
{
  File file = SPIFFS.open(filename, "a");              // open file in append mode 
  
  if(!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  Serial.println("Enter contents: ");

  while(!Serial.available());                          // wait for the user to enter the content
  String content = Serial.readStringUntil('\n');       // read the user input from serial monitor 
  // content.trim();                                      // remove any leading or trailing white spaces

  if(content.length() > (SPIFFS.totalBytes() - SPIFFS.usedBytes()))
  {
    Serial.println("Not enough space to write file");
    PrintCommands();
    return;
  }
  
  if(file.print(content))
    Serial.println("File written successfully");
  else 
    Serial.println("Failed to write file");
  
  file.close();
  PrintCommands();
}


void DeleteFile()
{
  if(SPIFFS.remove(filename))
    Serial.println("File deleted successfully");
  else 
    Serial.println("Failed to delete file");

  PrintCommands();
}

// erase all the data stored in SPIFFS 
void FormatSPIFFS()
{
  if(SPIFFS.format())
    Serial.println("SPIFFS formated successfully");
  else 
    Serial.println("Failed to format SPIFFS");

  PrintCommands();
}


void DownloadFile()
{
  HTTPClient http;

  Serial.println("Enter the URL of the file");

  while(!Serial.available());                          // wait for the user to enter the content
  String fileurl = Serial.readStringUntil('\n');       // read the user input from serial monitor 
  fileurl.trim();                                      // remove any leading or trailing white spaces

  http.begin(fileurl);

  int httpCode = http.GET();

  if(httpCode == HTTP_CODE_OK)
  {
    int filesize = http.getSize();
    if(filesize > (SPIFFS.totalBytes() - SPIFFS.usedBytes()))        // check for available storage space 
    {
      Serial.println("Not Enought space to download file");
      PrintCommands();
      return;      
    }

    Serial.printf("Downloading file (%d Bytes)", filesize);

    File file = SPIFFS.open(filename, "w");
    if(!file)
    {
      Serial.println("- failed to open file for writing");
      PrintCommands();
      return;
    }
    File log = SPIFFS.open("/Logs", "a");
    if(!log)
    {
      Serial.println("- failed to open log file for writing");
      PrintCommands();
      return;
    }

    http.setTimeout(100000);         // setting timeout to 100 sec

    WiFiClient *stream = http.getStreamPtr();

    uint8_t buffer[BUFFER_SIZE];                              // buffer to store chunks of file 
    int downloaded_data_size = 0;
    float endtime;
    String logs = "\n\nStarted Downloading " + filename + " at " + String(millis()) + " ms \n";
    log.println(logs);
    float starttime = millis();              // start of download and write time measurement 
    
    while(downloaded_data_size < filesize)
    {
      int available_data_size = stream -> available();          

      if(available_data_size > 0)                    // loop unitl data is availble
      {
        int readSize = (available_data_size > BUFFER_SIZE) ? BUFFER_SIZE : available_data_size;// setting readsize to max of buffer size 

        stream -> readBytes(buffer, readSize);        // read data from stream and store into buffer

        file.write(buffer, readSize);                //  read data from buffer and store into file
        
        logs = String(millis()) + " ms : " + readSize + " Bytes written ";
        log.println(logs);

        downloaded_data_size += readSize;
      }      
    }
    endtime = millis();                  // end of download and write time measurement
    file.close();

    logs = "\n\nCompleted downloading " + filename + " at " + String(endtime) + " ms";
    log.println(logs);

    float downloadSpeed = (filesize / ((endtime - starttime) / 1000)) / 1024;

    Serial.println("\n\nSuccessfully downloaded file");
    Serial.printf("\nDownload speed = %.2f KBps (Kilo Bytes per sec) ; %.2f Kbps (Kilo bits per sec", downloadSpeed, downloadSpeed*8);

    logs = "Download speed = " + String(downloadSpeed) + " KBps (Kilo Bytes per sec)";
    log.println(logs);
    logs = "\n\n****************************************************************************";
    log.println(logs);
    
    log.close();
    
    PrintCommands();
  }
  else
  {
    Serial.printf("Error downloding file : %d", httpCode);
    PrintCommands();
    return;
  }
}




