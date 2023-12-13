#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

std::string nickname;

void ReceiveMessages(int socket) {
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
    // Инициализация Winsock (не требуется на Linux)

    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    if (connect(client, reinterpret_cast<struct sockaddr*>(&hint), sizeof(hint)) == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(client);
        // WSACleanup(); (не требуется на Linux)
        return -1;
    }

    std::cout << "Enter your nickname: ";
    std::getline(std::cin, nickname);
    send(client, nickname.c_str(), nickname.size(), 0);

    std::thread(ReceiveMessages, client).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (message == "/quit") {
            send(client, message.c_str(), message.size(), 0);
            break;
        }

        send(client, message.c_str(), message.size(), 0);
    }

    close(client);
    // WSACleanup(); (не требуется на Linux)

    std::cout << std::endl << "You have left the chat." << std::endl;

    return 0;
}
