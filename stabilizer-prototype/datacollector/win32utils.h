#pragma once

#include <string>
#include <Windows.h>

std::string getErrorCodeString(DWORD errorCode);

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string getLastErrorAsString();

void reportWin32Error(unsigned long errorCode, const std::string &msg = "");