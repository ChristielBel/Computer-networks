#include <iostream>
#include <cstring>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// Структура для передачи данных от клиента к серверу
struct StudentData {
    char lastName[256];
    int grades[4];
};

// Структура для хранения результата стипендии и количества задолженностей
struct ScholarshipResult {
    std::string scholarship;
    int debtCount;
};

// Функция для вычисления стипендии
ScholarshipResult calculateScholarship(const StudentData& student) {
    bool hasDebt = false;
    bool hasThree = false;
    int debtCount = 0;

    for (int i = 0; i < 4; ++i) {
        if (student.grades[i] == 2) {
            hasDebt = true;
            debtCount++;
        } else if (student.grades[i] == 3) {
            hasThree = true;
        }
    }

    if (hasDebt) {
        return {"No scholarship due to debt.", debtCount};
    } else if (hasThree) {
        return {"No scholarship due to grade 3.", 0};
    } else if (std::all_of(student.grades, student.grades + 4, [](int grade) { return grade == 5; })) {
        return {"Increased scholarship.", 0};
    } else {
        return {"Regular scholarship.", 0};
    }
}

// Функция для обработки запросов клиента
void handleClient(SOCKET clientSocket) {
    std::cout << "Client connected.\n";

    while (true) {
        StudentData student;
        int bytesRead = recv(clientSocket, reinterpret_cast<char*>(&student), sizeof(StudentData), 0);

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

        std::string response;
        if (!validGrades) {
            response = "Invalid grades. Grades must be in the range of 2 to 5.";
        } else {
            ScholarshipResult result = calculateScholarship(student);
            response = result.scholarship;

            if (result.debtCount > 0) {
                response += " (Debts: " + std::to_string(result.debtCount) + ")";
            }
        }

        send(clientSocket, response.c_str(), response.size() + 1, 0);
    }

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Server listening on port 12345...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);

        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
