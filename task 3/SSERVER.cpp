// UDP-эхо сервер
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

struct Person
{
    int grades[4];  //оценки
    char name[500];  //имя
}B;

int main() {
    setlocale(LC_ALL, "RUS");
    char buff[1024];
    int answer=-1;
    cout << "UDP DEMO ECHO-SERVER\n";
    // шаг 1 - подключение библиотеки 
    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        cout << "WSAStartup error: " << WSAGetLastError();
        return -1;
    }
    // шаг 2 - создание сокета
    SOCKET Lsock;
    Lsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (Lsock == INVALID_SOCKET) {
        cout << "SOCKET() ERROR: " << WSAGetLastError();
        WSACleanup();
        return -1;
    }
    // шаг 3 - связывание сокета с локальным адресом 
    sockaddr_in Laddr;
    Laddr.sin_family = AF_INET;
    Laddr.sin_addr.s_addr = INADDR_ANY;   // или 0 (любой IP адрес)
    Laddr.sin_port = htons(PORT);
    if (bind(Lsock, (sockaddr*)&Laddr, sizeof(Laddr)))
    {
        cout << "BIND ERROR:" << WSAGetLastError();
        closesocket(Lsock);
        WSACleanup();
        return -1;
    }
    // шаг 4 обработка пакетов, присланных клиентами
    while (1) {
        sockaddr_in    Caddr;
        int Caddr_size = sizeof(Caddr);
        int bsize = recvfrom(Lsock, (char*)&B, sizeof(B), 0,(sockaddr*)&Caddr, &Caddr_size);
        if (bsize == SOCKET_ERROR)
            cout << "RECVFROM() ERROR:" << WSAGetLastError();
        // Определяем IP-адрес клиента и прочие атрибуты
        HOSTENT* hst;
        hst = gethostbyaddr((char*)&Caddr.sin_addr, 4, AF_INET);
        cout << "NEW DATAGRAM!" << endl;
        cout << ((hst) ? hst->h_name : "Unknown host\n") << endl;
        cout << inet_ntoa(Caddr.sin_addr) << endl;
        cout << ntohs(Caddr.sin_port) << endl;
       /* buff[bsize] = '\0';  */            // добавление завершающего нуля
        cout << "C=>S:"; /*<< buff << '\n';*/        // Вывод на экран 
        cout << "Name:" << B.name << endl;
        cout << "Grades: ";
        for (int i = 0; i < 4; i++)
            cout << B.grades[i] << " ";
        cout << endl;
        if (B.grades[0] == 2 || B.grades[1] == 2 || B.grades[2] == 2 || B.grades[3] == 2) answer = 2;
        else if (B.grades[0] == 3 || B.grades[1] == 3 || B.grades[2] == 3 || B.grades[3] == 3) answer = 3;
        else if (B.grades[0] == 5 && B.grades[1] == 5 && B.grades[2] == 5 && B.grades[3] == 5) answer = 5;
        else answer = 4;
        switch (answer) {
        case 2: {cout << "Задолженность.Стипендия не выплачивается\n"; break; }
        case 3: {cout << "Стипендия не выплачивается\n"; break; }
        case 4: {cout << "1000 \t Обычная стипендия\n"; break; }
        case 5: {cout << "2000 \t Повышенная стипендия\n"; break; }
        case -1: {cout << "Error, server sent wrong data"; }
        }
// посылка датаграммы клиенту
        sendto(Lsock, (char*)&answer, sizeof(answer), 0, (sockaddr*)&Caddr, sizeof(Caddr));
    }      return 0;
}
