#include <iostream>
#include <thread>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::string nickname;

//Функция ReceiveMessages в потоке клиента слушает сообщения 
//от сервера и выводит их в консоль. 
//При отключении сервера выводится сообщение об этом.
void ReceiveMessages(SOCKET socket) {
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Server disconnected" << std::endl;
            break;
        }

        std::string message(buffer, 0, bytesReceived);
        std::cout << message /*<< std::endl*/;
    }
}

int main() {
    //Инициализация Winsock:
    //Клиент инициализирует Winsock и подключается к серверу, указанному IP-адресу и порту.
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return -1;
    }

    SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    if (connect(client, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(client);
        WSACleanup();
        return -1;
    }

    //Получение никнейма и создание потока для приема сообщений:
    //Клиент вводит свой никнейм, 
    //отправляет его на сервер и создает поток для приема сообщений от сервера.
    std::cout << "Enter your nickname: ";
    std::getline(std::cin, nickname);
    send(client, nickname.c_str(), nickname.size(), 0);

    std::thread(ReceiveMessages, client).detach();

    //Отправка сообщений на сервер:
    //Клиент в бесконечном цикле вводит сообщения с клавиатуры и отправляет их на сервер. 
    //Если введено "/quit", клиент отправляет это сообщение и завершает работу.
    //Для отправки личных сообщений используется /w имя_пользователя сообщение
    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (message == "/quit") {
            send(client, message.c_str(), message.size(), 0);
            break;
        }

        send(client, message.c_str(), message.size(), 0);
    }

    closesocket(client);
    WSACleanup();

    std::cout << std::endl << "You have left the chat." << std::endl;

    return 0;
}
