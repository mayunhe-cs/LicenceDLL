#pragma once
#include <Windows.h>
#include <string>
using std::string;

#define MAX_SIZE 50
#define BUF_SIZE 50

string base64_encode(unsigned char const*, unsigned int len);
string base64_decode(string const& s);

