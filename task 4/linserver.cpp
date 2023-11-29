#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using std::cerr;

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        perror("Error creating socket");
        return 1;
    }

    // Инициализация структуры для адреса сервера
    // HTTP-сервер на 8000-м порту localhost
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8000);

    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(listen_socket);
        return 1;
    }

    if (listen(listen_socket, SOMAXCONN) == -1) {
        perror("Listen failed");
        close(listen_socket);
        return 1;
    }

    const int max_client_buffer_size = 1024;
    char buf[max_client_buffer_size];
    int client_socket = -1;

    for (;;) {
        client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accept failed");
            close(listen_socket);
            return 1;
        }

        int result = recv(client_socket, buf, max_client_buffer_size, 0);
        std::stringstream response;
        std::stringstream response_body;

        if (result == -1) {
            cerr << "recv failed\n";
            close(client_socket);
        } else if (result == 0) {
            cerr << "connection closed...\n";
        } else if (result > 0) {
            buf[result] = '\0';

            response_body << "<title>Test C++ HTTP Server</title>\n"
                          << "<h1>Test page</h1>\n"
                          << "<p>This is the body of the test page...</p>\n"
                          << "<h2>Request headers</h2>\n"
                          << "<pre>" << buf << "</pre>\n"
                          << "<em><small>Test C++ Http Server</small></em>\n";

            response << "HTTP/1.1 200 OK\r\n"
                     << "Version: HTTP/1.1\r\n"
                     << "Content-Type: text/html; charset=utf-8\r\n"
                     << "Content-Length: " << response_body.str().length()
                     << "\r\n\r\n"
                     << response_body.str();

            result = send(client_socket, response.str().c_str(), response.str().length(), 0);
            if (result == -1) {
                cerr << "send failed\n";
            }

            close(client_socket);
        }
    }

    close(listen_socket);
    return 0;
}


// sudo telnet localhost 8000
// запр
