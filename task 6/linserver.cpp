#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <algorithm>

// Структура, представляющая информацию о клиенте
struct ClientInfo {
    int socket;
    std::string nickname;
};

// Вектор для хранения информации о клиентах
std::vector<ClientInfo> clients;
// Мьютекс для безопасного доступа к вектору clients
std::mutex clientsMutex;
// Счетчик подключенных клиентов
int connectedClients = 0;

// Функция для отправки сообщения всем клиентам
void BroadcastMessage(const std::string& message, const std::string& senderNickname = "") {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto it = clients.begin(); it != clients.end();) {
        int clientSocket = it->socket;
        std::string formattedMessage = "[" + (senderNickname.empty() ? "Server" : senderNickname) + "]: " + message + "\n";

        ssize_t result = send(clientSocket, formattedMessage.c_str(), formattedMessage.size(), 0);
        if (result == -1) {
            std::cerr << "Failed to send message to client" << std::endl;
            it = clients.erase(it);
            close(clientSocket);
            connectedClients = static_cast<int>(clients.size());
            std::cout << "Connected clients: " << connectedClients << std::endl;
        } else {
            ++it;
        }
    }
}

// Функция для обработки сообщений от клиента
void HandleClient(ClientInfo client) {
    char buffer[4096];
    std::string welcomeMessage = "[Server]: Welcome, " + client.nickname + "!";
    welcomeMessage += " Use '/w username message' to send a private message. Use '/quit' to exit the chat.\n";
    send(client.socket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
    }

    while (true) {
        int bytesReceived = recv(client.socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = std::remove_if(clients.begin(), clients.end(),
                                     [client](const ClientInfo& c) { return c.socket == client.socket; });
            if (it != clients.end()) {
                std::cout << "Client " << client.nickname << " disconnected" << std::endl;
                close(client.socket);
                std::string leaveMessage = "[System]: User " + client.nickname + " has left the chat.\n";
                for (const auto& otherClient : clients) {
                    if (otherClient.socket != client.socket) {
                        send(otherClient.socket, leaveMessage.c_str(), leaveMessage.size(), 0);
                    }
                }
                clients.erase(it);
                --connectedClients;
                std::cout << "Connected clients: " << connectedClients << std::endl;
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
                        std::string formattedPrivateMessage = "[Private from " + client.nickname + "]: " + privateMessage + "\n";
                        ssize_t result = send(targetClient.socket, formattedPrivateMessage.c_str(), formattedPrivateMessage.size(), 0);
                        if (result == -1) {
                            std::cerr << "Failed to send private message to client" << std::endl;
                        }
                        break;
                    }
                }
            }
        } else {
            if (message != "/quit") {
                BroadcastMessage(message, client.nickname);
            }
        }
    }
}

int main() {

    // Создание сокета и настройка прослушивания
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY;

    bind(listening, reinterpret_cast<struct sockaddr*>(&hint), sizeof(hint));

    listen(listening, SOMAXCONN);

    std::cout << "Server is listening..." << std::endl;

    // Ожидание и обработка подключений
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);

        int clientSocket = accept(listening, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientSize);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        memset(host, 0, NI_MAXHOST);
        memset(service, 0, NI_MAXSERV);

        if (getnameinfo(reinterpret_cast<struct sockaddr*>(&clientAddr), sizeof(clientAddr),
                        host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << "New client connected! " << std::endl;
            ++connectedClients;
            std::cout << "Connected clients: " << connectedClients << std::endl;
        } else {
            inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
            std::cout << "New client connected! " << std::endl;
            ++connectedClients;
            std::cout << "Connected clients: " << connectedClients << std::endl;
        }

        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::string nickname(buffer, 0, bytesReceived);

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back({clientSocket, nickname});
        }

        BroadcastMessage("User " + nickname + " has joined the chat.");

        std::thread(HandleClient, clients.back()).detach();
    }

    return 0;
}
