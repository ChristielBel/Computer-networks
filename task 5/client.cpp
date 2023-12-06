#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

struct StudentData {
    char lastName[50];
    int grades[5];
};

int main() {
    // Создаем сокет
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error in socket creation.\n";
        return -1;
    }

    // Настраиваем адрес сервера
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

    // Подключаемся к серверу
    if (connect(client_socket, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address)) == -1) {
        std::cerr << "Error in connection.\n";
        close(client_socket);
        return -1;
    }

    std::cout << "Connected to the server. Type 'quit' to exit.\n";

    while (true) {
        StudentData studentData;

        // Вводим данные студента
        std::cout << "Enter last name: ";
        std::cin.getline(studentData.lastName, sizeof(studentData.lastName));

        std::cout << "Enter grades (5 numbers separated by space): ";
        for (int i = 0; i < 5; ++i) {
            std::cin >> studentData.grades[i];
        }

        // Отправляем данные на сервер
        send(client_socket, &studentData, sizeof(StudentData), 0);

        // Проверяем, если введена команда "quit", то закрываем соединение
        if (strncmp(studentData.lastName, "quit", 4) == 0) {
            std::cout << "Closing the connection.\n";
            break;
        }

        // Получаем ответ от сервера
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cerr << "Error in receiving data or connection closed.\n";
            break;
        }

        // Выводим ответ от сервера
        std::cout << "Server response: " << std::string(buffer) << "\n";
    }

    // Закрываем клиентский сокет
    close(client_socket);

    return 0;
}
