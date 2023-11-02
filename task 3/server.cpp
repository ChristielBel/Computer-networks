// UDP-эхо сервер
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;
  #define PORT 666    // порт сервера
  #define sHELLO "Hello, STUDENT\n"
  int main()     {
    char buff[1024];
    cout<< "UDP DEMO ECHO-SERVER\n";
// шаг 1 - подключение библиотеки 
    if (WSAStartup(0x202,(WSADATA *) &buff[0]))
    {
      cout<< "WSAStartup error: "<< WSAGetLastError();
      return -1;     }
