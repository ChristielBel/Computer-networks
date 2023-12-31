// пример UDP-клиента
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;
#define PORT 666
#define SERVERADDR "127.0.0.1" // IP-адрес сервера

struct Person
{
    int grades[4];  //оценки
    char name[500];  //имя
} A;

int main()
{
    setlocale(LC_ALL, "RUS");
    char buff[10 * 1014];
    int answer=-1;
    cout << "UDP Demo Client\nType quit to quit \n";
    // Шаг 1 - иницилизация библиотеки Winsocks
    if (WSAStartup(0x202, (WSADATA*)&buff))
    {
        cout << "WSASTARTUP ERROR: " << WSAGetLastError() << "\n";
        return -1;
    }
    // Шаг 2 - открытие сокета
    SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock == INVALID_SOCKET) {
        cout << "SOCKET() ERROR: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }
    // Шаг 3 - обмен сообщений с сервером
    HOSTENT* hst;
    sockaddr_in Daddr;
    Daddr.sin_family = AF_INET;
    Daddr.sin_port = htons(PORT);

    // определение IP-адреса узла
    if (inet_addr(SERVERADDR))
        Daddr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else
        if (hst = gethostbyname(SERVERADDR))
            Daddr.sin_addr.s_addr = ((unsigned long**)
                hst->h_addr_list)[0][0];
        else {
            cout << "Unknown Host: " << WSAGetLastError() << "\n";
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    while (1) {
        // чтение сообщения с клавиатуры
        cout << "S<=C:" << endl;
        /*string SS;
        getline(cin, SS);
        if (SS == "quit") break;*/
        cout << "System: Введите данные: Фамилия Оценки(4 штуки)" << endl;
        cout << "you:";
        cin >> A.name;
        for (int i = 0; i < 4; i++)
            cin >> A.grades[i];
        // Передача сообщений на сервер
        sendto(my_sock, (char*)&A, sizeof(A), 0,(sockaddr*)&Daddr, sizeof(Daddr));
        // Прием сообщения с сервера
        sockaddr_in SRaddr;
        int SRaddr_size = sizeof(SRaddr);
        int n = recvfrom(my_sock, (char*)&answer, sizeof(answer), 0,(sockaddr*)&SRaddr, &SRaddr_size);
        if (n == SOCKET_ERROR) {
            cout << "RECVFROM() ERROR:" << WSAGetLastError() << "\n";
            closesocket(my_sock);
            WSACleanup();  return -1;
        }
        buff[n] = '\0';
        // Вывод принятого с сервера сообщения на экран
        cout << "S=>C:";
        switch (answer) {
        case 2: {cout << "Задолженность.Стипендия не выплачивается\n"; break; }
        case 3: {cout << "Стипендия не выплачивается\n"; break; }
        case 4: {cout << "1000 \t Обычная стипендия\n"; break; }
        case 5: {cout << "2000 \t Повышенная стипендия\n"; break; }
        case -1: {cout << "Error, server sent wrong data"; }
        }
    }
    // шаг последний - выход
    closesocket(my_sock);
    WSACleanup();
    return 0;
}
