#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/select.h>
#include <map>


int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        return 1;
    }

    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Listening failed." << std::endl;
        return 1;
    }

    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(serverSocket, &readFds);
    int maxFd = serverSocket;

    int step_counter = 0;
    std::map<int, std::string> clients;
    bool STEP = false;
    while (true) {
        fd_set currentFds = readFds;
        if (select(maxFd + 1, &currentFds, nullptr, nullptr, nullptr) < 0) {
            std::cerr << "Select failed." << std::endl;
            return 1;
        }

        for (int fd = 0; fd <= maxFd; ++fd) {
            if (FD_ISSET(fd, &currentFds)) {
                if (fd == serverSocket) {
                    // New connection
                    // STEP = false;

                    int clientSocket = accept(serverSocket, nullptr, nullptr);
                    if (clientSocket < 0) {
                        std::cerr << "Failed to accept connection." << std::endl;
                        return 1;
                    }

                    FD_SET(clientSocket, &readFds);
                    clients[clientSocket] = "";
                    maxFd = std::max(maxFd, clientSocket);
                    std::cout << "Accepted new connection on socket " << clientSocket << std::endl;

                } else {
                    char buffer[1024] = {0};
                    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
                    if (bytesRead < 0) {
                        std::cerr << "Read failed." << std::endl;
                        return 1;
                    }

                    clients[fd] = buffer;

                    if (clients[fd] == "Python Client") {
                        STEP = true;
                        std::cout << "STEP from Python: " << STEP << std::endl;
                        step_counter++;
                        std::cout << "STEPCOUNTER: " << step_counter << std::endl;

                        // for(const auto& client : clients) {
                        //     if (client.second == "C++ Client") {
                        //         const char *response = "Step";
                        //         if (send(client.first, response, strlen(response), 0) < 0) {
                        //             std::cerr << "Send failed." << std::endl;
                        //             return 1;
                        //         }
                        //     }
                        // }
                    }

                    else if (clients[fd] == "C++ Client") {
                        const std::string response = std::string("Step = ") + ((step_counter > 0) ? "true, " : "false, ") + "Number: " + std::to_string(step_counter);
                        // const std::string response = "Step = " + ((step_counter > 0) ? "true, " : "false, ") + "Number: " + std::to_string(step_counter);
                        if (send(fd, response.c_str(), response.length(), 0) < 0) {
                            std::cerr << "Send failed." << std::endl;
                            return 1;
                        }
                        if (step_counter > 0) {
                            step_counter--;
                        }
                    }





                    close(fd);
                    FD_CLR(fd, &readFds);
                    std::cout << "Closed connection on socket " << fd << std::endl;
                }
            }
        }
    }

    close(serverSocket);
    return 0;
}