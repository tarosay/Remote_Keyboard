#ifndef _FFS_HPP_
#define _FFS_HPP_

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

bool FFS_begin();
bool SSID_Write(String data);
bool PASSWORD_Write(String data);
String SSID_Read();
String PASSWORD_Read();

#endif  // _FFS_HPP_