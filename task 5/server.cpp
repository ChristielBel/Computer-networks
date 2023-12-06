#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

struct StudentData {
    char lastName[50];
    int grades[5];
};

void handle_client(int client_socket) {
    StudentData studentData;
    int bytes_received;

    while (true) {
        // Получаем данные от клиента
        bytes_received = recv(client_socket, &studentData, sizeof(StudentData), 0);
        if (bytes_received <= 0) {
            std::cerr << "Error in receiving data or connection closed.\n";
            break;
        }

        // Проверяем, если клиент отправил "quit", то разрываем соединение
        if (strncmp(studentData.lastName, "quit", 4) == 0) {
            std::cout << "Client requested to close the connection. Closing...\n";
            break;
        }

        // Вычисляем средний балл
        double averageGrade = 0.0;
        for (int i = 0; i < 5; ++i) {
            averageGrade += studentData.grades[i];
        }
        averageGrade /= 5.0;

        // Определяем стипендию
        std::string scholarship;
        if (averageGrade >= 4.5) {
            scholarship = "Full scholarship";
        } else if (averageGrade >= 4.0) {
            scholarship = "Partial scholarship";
        } else {
            scholarship = "No scholarship";
        }

        // Отправляем результат обратно клиенту
        send(client_socket, scholarship.c_str(), scholarship.length() + 1, 0);
    }

    // Закрываем соединение с клиентом
    close(client_socket);
}

int main() {
    // Создаем сокет
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error in socket creation.\n";
        return -1;
    }

    // Настраиваем адрес сервера
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // Связываем сокет с адресом сервера
    if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address)) == -1) {
        std::cerr << "Error in binding.\n";
        close(server_socket);
        return -1;
    }

    // Начинаем слушать запросы
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Error in listening.\n";
        close(server_socket);
        return -1;
    }

    std::cout << "Server listening on port 8080...\n";

    while (true) {
        // Принимаем новое соединение
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) {
            std::cerr << "Error in accepting connection.\n";
            continue;
        }

        // Запускаем новый поток для обработки клиента
        std::thread(handle_client, client_socket).detach();
    }

    // Закрываем серверный сокет
    close(server_socket);

    return 0;
}
