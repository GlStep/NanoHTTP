#include <cstring>
#include <iostream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSE_SOCKET closesocket
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#define CLOSE_SOCKET close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

bool initializeSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void handleClient(socket_t clientSocket) {
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        std::string body = "<html><body><h1>Hello, World!</h1></body></html>";
        std::string httpResponse =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " +
            std::to_string(body.size()) +
            "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;
        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    if (!initializeSockets()) {
        std::cerr << "Failed to initialize sockets." << std::endl;
        return 1;
    }

    // Creating socket
    socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        cleanupSockets();
        return 1;
    }

    // Configure server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        CLOSE_SOCKET(serverSocket);
        cleanupSockets();
        return 1;
    }

    std::cout << "Sockets initialized successfully." << std::endl;

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        CLOSE_SOCKET(serverSocket);
        cleanupSockets();
        return 1;
    }

    std::cout << "Server is listening on port 8080: http://localhost:8080" << std::endl;

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        socket_t clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            break;
        }

        std::cout << "Client connected." << std::endl;

        // TODO: Handle client communication
        handleClient(clientSocket);
        CLOSE_SOCKET(clientSocket);
    }

    CLOSE_SOCKET(serverSocket);
    cleanupSockets();

    return 0;
}
