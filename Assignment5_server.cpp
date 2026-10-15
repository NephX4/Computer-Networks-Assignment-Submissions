#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 100

using namespace std;

class Chat
{
public:
    void sendMessage(int sockfd, const char *message)
    {
        send(sockfd, message, strlen(message), 0); // 0 = default sending behavior
    }

    // returns number of bytes received (<= 0 means disconnected / error)
    int receiveMessage(int sockfd, char *message)
    {
        int n = recv(sockfd, message, BUF_SIZE - 1, 0);
        if (n > 0)
            message[n] = '\0';
        return n;
    }
};

int main()
{
    // Don't let a write to a closed socket kill the program
    signal(SIGPIPE, SIG_IGN);

    // Step 1: Create a socket (no WSAStartup needed on macOS/Linux)
    // AF_INET = IPv4, SOCK_STREAM = stream socket (TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "\nSocket creation failed";
        return 1;
    }

    // Lets you restart the server right away without "Bind failed"
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Step 2: Server and client address structures, zero-initialised
    sockaddr_in serverAddr = {}, clientAddr = {};

    // Step 3: Configure server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // accept connections on all local IPv4 interfaces
    serverAddr.sin_port = htons(PORT);       // host byte order -> network byte order

    // Step 4: Bind the socket to the address and port
    // (::bind avoids a clash with std::bind because of "using namespace std")
    if (::bind(sockfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "\nBind failed";
        close(sockfd);
        return 1;
    }

    // Step 5: Listen for connections (5 = max queued connections)
    if (listen(sockfd, 5) < 0)
    {
        cout << "\nListen failed";
        close(sockfd);
        return 1;
    }
    cout << "\nServer started...";
    cout << "\nWaiting for the client..." << flush;

    // Step 6: Accept the client connection (socklen_t instead of int)
    socklen_t len = sizeof(clientAddr);
    int connfd = accept(sockfd, (sockaddr *)&clientAddr, &len);
    if (connfd < 0)
    {
        cout << "\nAccept failed";
        close(sockfd);
        return 1;
    }
    cout << "\nClient connected!";

    // Step 7: Chat
    char buffer[BUF_SIZE];
    Chat c1;
    while (true)
    {
        int n = c1.receiveMessage(connfd, buffer);
        if (n <= 0)
        {
            cout << "\nClient disconnected.";
            break;
        }
        cout << "\nClient: " << buffer;
        if (strcmp("bye", buffer) == 0)
        {
            cout << "\nClient ended communication!";
            break;
        }

        cout << "\nServer: " << flush;
        cin.getline(buffer, BUF_SIZE);
        if (cin.eof())
            strcpy(buffer, "bye"); // Ctrl+D: quit cleanly
        c1.sendMessage(connfd, buffer);
        if (strcmp(buffer, "bye") == 0)
        {
            cout << "\nServer ended the communication!";
            break;
        }
    }

    cout << endl;
    close(connfd);
    close(sockfd);
    return 0;
}
