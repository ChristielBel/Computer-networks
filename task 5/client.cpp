#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

// Структура для передачи данных от клиента к серверу
struct StudentData {
    char lastName[256];
    int grades[4];
};

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес сервера

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to the server.\n";
        return 1;
    }

    while (true) {
        StudentData student;

        std::cout << "Enter last name (or 'quit' to exit): ";
        std::cin >> student.lastName;

        if (strcmp(student.lastName, "quit") == 0) {
            send(clientSocket, &student, sizeof(StudentData), 0);
            break;
        }

        std::cout << "Enter 4 grades: ";
        for (int i = 0; i < 4; ++i) {
            std::cin >> student.grades[i];
        }

        send(clientSocket, &student, sizeof(StudentData), 0);

        char buffer[1024];
        recv(clientSocket, buffer, sizeof(buffer), 0);

        std::cout << "Server response: " << buffer << std::endl;
    }

    close(clientSocket);
    return 0;
}
