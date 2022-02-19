#pragma once
#include <Windows.h>
#define MAX_SIZE 50
#define BUF_SIZE 50

UINT GetAdapterCharacteristics(char* adapter_name);
int GetMAC(BYTE mac[BUF_SIZE]);
void GetMacAddress(char* mac);