#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <string>
#include <winsock2.h>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

struct StudentData {
    char lastName[256];
    int grades[4];
};

struct ScholarshipResult {
    std::string scholarship;
    int debtCount;
};

ScholarshipResult calculateScholarship(const StudentData& student) {
    bool hasDebt = false;
    bool hasThree = false;
    int debtCount = 0;

    for (int i = 0; i < 4; ++i) {
        if (student.grades[i] == 2) {
            hasDebt = true;
            debtCount++;
        }
        else if (student.grades[i] == 3) {
            hasThree = true;
        }
    }

    if (hasDebt) {
        return { "No scholarship due to debt.", debtCount };
    }
    else if (hasThree) {
        return { "No scholarship due to grade 3.", 0 };
    }
    else if (std::all_of(student.grades, student.grades + 4, [](int grade) { return grade == 5; })) {
        return { "Increased scholarship.", 0 };
    }
    else {
        return { "Regular scholarship.", 0 };
    }
}

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

        ScholarshipResult result = calculateScholarship(student);
        std::string response = result.scholarship;

        if (result.debtCount > 0) {
            response += " (Debts: " + std::to_string(result.debtCount) + ")";
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

    std::vector<std::thread> threads;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        threads.emplace_back(handleClient, clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
