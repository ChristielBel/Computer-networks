#include <iostream>
#include <cstring>
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS  
#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")

struct StudentData {
    char lastName[256];
    int grades[4];
};

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid address. Error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to the server: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        StudentData student;

        std::cout << "Enter last name (or 'quit' to exit): ";
        std::cin >> student.lastName;

        if (strcmp(student.lastName, "quit") == 0) {
            send(clientSocket, reinterpret_cast<char*>(&student), sizeof(StudentData), 0);
            closesocket(clientSocket);  
            break;
        }

        std::cout << "Enter 4 grades: ";
        for (int i = 0; i < 4; ++i) {
            std::cin >> student.grades[i];
        }

        send(clientSocket, reinterpret_cast<char*>(&student), sizeof(StudentData), 0);

        char buffer[1024];
        recv(clientSocket, buffer, sizeof(buffer), 0);

        std::cout << "Server response: " << buffer << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
