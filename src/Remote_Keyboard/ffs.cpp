#include "ffs.hpp"

#define SSID_FILEPATH "/ssid.txt"
#define PASSWORD_FILEPATH "/password.txt"
#define FORMAT_SPIFFS_IF_FAILED true

bool FFS_begin() {
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    return false;
  }
  return true;
}

bool FWrite(String filePath, String data) {
  File file = SPIFFS.open((const char*)filePath.c_str(), FILE_WRITE);
  if (!file) {
    return false;
  }
  file.write((const unsigned char*)data.c_str(), data.length());
  file.close();
  return true;
}

String FRead(String filePath) {
  File file = SPIFFS.open(filePath);
  if (!file) {
    return "";
  }
  if (file.isDirectory()) {
    file.close();
    return "";
  }
  String data = "";
  while (file.available()) {
    char c = file.read();
    data += c;
  }
  file.close();
  return data;
}

bool SSID_Write(String data) {
  return FWrite(SSID_FILEPATH, data);
}
bool PASSWORD_Write(String data) {
  return FWrite(PASSWORD_FILEPATH, data);
}
String SSID_Read() {
  return FRead(SSID_FILEPATH);
}
String PASSWORD_Read() {
  return FRead(PASSWORD_FILEPATH);
}
