#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <algorithm>

// Структура для передачи данных от клиента к серверу
struct StudentData {
    char lastName[256];
    int grades[4];
};

// Функция для вычисления стипендии
std::string calculateScholarship(const StudentData& student) {
    bool hasDebt = false;
    bool hasThree = false;

    for (int i = 0; i < 4; ++i) {
        if (student.grades[i] == 2) {
            hasDebt = true;
            break;
        } else if (student.grades[i] == 3) {
            hasThree = true;
        }
    }

    if (hasDebt) {
        return "No scholarship due to debt.";
    } else if (hasThree) {
        return "No scholarship due to grade 3.";
    } else if (std::all_of(student.grades, student.grades + 4, [](int grade) { return grade == 5; })) {
        return "Increased scholarship.";
    } else {
        return "Regular scholarship.";
    }
}

// Функция для обработки запросов клиента
void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);
    delete (int*)arg;

    std::cout << "Client connected.\n";

    while (true) {
        StudentData student;
        int bytesRead = recv(clientSocket, &student, sizeof(StudentData), 0);

        if (bytesRead <= 0) {
            std::cout << "Connection closed by client.\n";
            break;
        }

        if (strcmp(student.lastName, "quit") == 0) {
            std::cout << "Client requested to quit. Closing connection.\n";
            break;
        }

        // Проверка корректности данных оценок
        bool validGrades = true;
        for (int i = 0; i < 4; ++i) {
            if (student.grades[i] < 2 || student.grades[i] > 5) {
                validGrades = false;
                break;
            }
        }

        if(validGrades==false) {
            std::string response;

            if (!validGrades) {
                response = "Invalid grades. Grades must be in the range of 2 to 5.";
            } else {
                response = calculateScholarship(student);
            }

            send(clientSocket, response.c_str(), response.size() + 1, 0);
        }
        std::string scholarship = calculateScholarship(student);
        send(clientSocket, scholarship.c_str(), scholarship.size() + 1, 0);
    }

    close(clientSocket);
    return NULL;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Server listening on port 12345...\n";

    while (true) {
        int* clientSocket = new int;
        *clientSocket = accept(serverSocket, NULL, NULL);

        pthread_t thread;
        pthread_create(&thread, NULL, handleClient, (void*)clientSocket);
        pthread_detach(thread);
    }

    close(serverSocket);
    return 0;
}
