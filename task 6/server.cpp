#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <WinSock2.h>
#include <algorithm>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Структура, представляющая информацию о клиенте
struct ClientInfo {
    SOCKET socket;
    std::string nickname;
};

// Вектор для хранения информации о клиентах
std::vector<ClientInfo> clients;
// Мьютекс для безопасного доступа к вектору clients
std::mutex clientsMutex;

// Функция для отправки сообщения всем клиентам
void BroadcastMessage(const std::string& message, const std::string& senderNickname = "") {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto it = clients.begin(); it != clients.end();) {
        SOCKET clientSocket = it->socket;
        std::string formattedMessage = "[" + (senderNickname.empty() ? "Server" : senderNickname) + "]: " + message;

        int result = send(clientSocket, formattedMessage.c_str(), formattedMessage.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Failed to send message to client" << std::endl;
            it = clients.erase(it);
            closesocket(clientSocket);
        }
        else {
            ++it;
        }
    }
}

// Функция для обработки сообщений от клиента
void HandleClient(ClientInfo client) {
    char buffer[4096];
    // Приветственное сообщение для нового пользователя
    // Информация о командах для личных сообщений и выхода
    std::string welcomeMessage = "[Server]: Welcome, " + client.nickname + "!";
    welcomeMessage += " Use '/w username message' to send a private message. Use '/quit' to exit the chat.";
    send(client.socket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

    while (true) {
        //Обработка выхода клиента:
        int bytesReceived = recv(client.socket, buffer, sizeof(buffer), 0);
        //При обнаружении отключения клиента, сервер отправляет системное 
        //сообщение о выходе этого клиента всем остальным клиентам.
        if (bytesReceived <= 0) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = std::remove_if(clients.begin(), clients.end(),
                [client](const ClientInfo& c) { return c.socket == client.socket; });
            if (it != clients.end()) {
                std::cout << "Client " << client.nickname << " disconnected" << std::endl;
                closesocket(client.socket);

                // Системное сообщение о выходе клиента
                std::string leaveMessage = "[System]: User " + client.nickname + " has left the chat.";

                // Отправка системного сообщения каждому клиенту, кроме вышедшего
                for (const auto& otherClient : clients) {
                    if (otherClient.socket != client.socket) {
                        send(otherClient.socket, leaveMessage.c_str(), leaveMessage.size(), 0);
                    }
                }

                clients.erase(it);
            }
            break;
        }

        std::string message(buffer, 0, bytesReceived);

        if (message.substr(0, 3) == "/w ") {
            size_t spacePos = message.find(" ", 3);
            if (spacePos != std::string::npos) {
                std::string targetNickname = message.substr(3, spacePos - 3);
                std::string privateMessage = message.substr(spacePos + 1);

                std::lock_guard<std::mutex> lock(clientsMutex);
                for (const auto& targetClient : clients) {
                    if (targetClient.nickname == targetNickname) {
                        std::string formattedPrivateMessage = "[Private from " + client.nickname + "]: " + privateMessage;
                        int result = send(targetClient.socket, formattedPrivateMessage.c_str(), formattedPrivateMessage.size(), 0);
                        if (result == SOCKET_ERROR) {
                            std::cerr << "Failed to send private message to client" << std::endl;
                        }
                        break;
                    }
                }
            }
        }
        else {
            if (message != "/quit") {
                BroadcastMessage(message, client.nickname);
            }
        }
    }
}

int main() {
    //Инициализация Winsock:
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return -1;
    }

    //Создание сокета и настройка прослушивания:
    //Создается сокет (listening), устанавливается порт и адрес для прослушивания, 
    //а затем сокет привязывается и начинает прослушивание входящих соединений.
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    std::cout << "Server is listening..." << std::endl;

    //Ожидание и обработка подключений:
    //В бесконечном цикле сервер ожидает подключения новых клиентов (accept). 
    //После подключения создается новый поток (std::thread), 
    //который обрабатывает сообщения от данного клиента (HandleClient). 
    //Сам сервер сохраняет информацию о клиенте (сокет и никнейм) 
    //и отправляет сообщение о подключении всем клиентам.
    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(listening, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << "Client connected: " << host << " on port " << service << std::endl;
        }
        else {
            inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
            std::cout << "Client connected: " << host << " on port " << ntohs(clientAddr.sin_port) << std::endl;
        }

        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::string nickname(buffer, 0, bytesReceived);

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back({ clientSocket, nickname });
        }

        BroadcastMessage("User " + nickname + " has joined the chat.\n");

        std::thread(HandleClient, clients.back()).detach();
    }

    WSACleanup();

    return 0;
}
