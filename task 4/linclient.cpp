#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define request "GET/ HTTP/1.1\r\n HOST: localhost \r\n\r\n"
#define max_packet_size 65535

int main() {
    int s;
    struct sockaddr_in adr;

    // создаем сокет
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // заполняем поля структуры adr для использования ее в connect
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_port = htons(8000);

    // преобразуем localhost в IP-адрес
    if (inet_pton(AF_INET, "127.0.0.1", &(adr.sin_addr)) <= 0) {
        perror("Invalid address");
        close(s);
        return -1;
    }

    // устанавливаем соединение с сервером
    if (connect(s, (struct sockaddr*)&adr, sizeof(adr)) == -1) {
        perror("Connection failed");
        close(s);
        return -1;
    }

    // посылаем запрос серверу
    if (send(s, request, strlen(request), 0) == -1) {
        perror("Send failed");
        close(s);
        return -1;
    }

    // ждем ответа
    char buff[max_packet_size];
    int len;

    do {
        if ((len = recv(s, buff, max_packet_size, 0)) == -1) {
            perror("Receive failed");
            close(s);
            return -1;
        }
        for (int i = 0; i < len; i++) {
            std::cout << buff[i];
        }
    } while (len != 0);

    // закрываем соединение
    if (close(s) == -1) {
        perror("Socket closing failed");
        return -1;
    }

    std::cin.get();
    return 0;
}
