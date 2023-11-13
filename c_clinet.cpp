#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int getCounterStep() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        return -1;
    }

    std::cout << "Connected to the server." << std::endl;

    const char *message = "C++ Client";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        std::cerr << "Send failed." << std::endl;
        return -1;
    }

    int counter_step = 0;

    //////read from sever./////
    char buffer[1024] = {0};
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        std::cerr << "Read failed." << std::endl;
        return -1;
    }

    std::string server_message(buffer);

    size_t pos = server_message.find("Step = ");
    if (pos != std::string::npos) {
        bool step = (server_message.substr(pos + 7, 4) == "true");  // Extract the "true" or "false" string
        if (step) {
            counter_step++;
        }
    }

    std::cout << "Server response: " << server_message << ", Total steps: " << counter_step << std::endl;

    close(clientSocket);
    return counter_step;
}

int main() {
    int step = getCounterStep();
    if (step == -1) {
        std::cerr << "Failed to get step count." << std::endl;
        return 1;
    }
    if (!step){
        std::cout<<"hell yeah";
    }
    std::cout << "Total steps: " << step << std::endl;
    return 0;
}